#include "client.h"

bool send_data_str(void *kv_handle, char* buf){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    ctx.buf = buf;
    ctx.size = (int) strlen(buf) + 1;

    unsigned int ctx_lag = IBV_SEND_SIGNALED;
    if (ctx.size < MAX_INLINE) {
        ctx_lag = IBV_SEND_SIGNALED | IBV_SEND_INLINE;
    }
    if (flagged_pp_post_send(&ctx, ctx_lag)) {
        fprintf(stderr, "Client couldn't post send\n");
        return EXIT_FAILURE;
    }

    // wait for completion
    if (pp_wait_completions(&ctx, 1)) {
        fprintf(stderr, "Client couldn't wait for completions\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

bool send_ACK(kvHandle* kv_handle){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    unsigned int ctx_flag = IBV_SEND_SIGNALED | IBV_SEND_INLINE;
    int size = ctx.size;
    ctx.size = 1;
    if (flagged_pp_post_send(&ctx, ctx_flag)) {
        fprintf(stderr, "Server couldn't post send\n");
        return EXIT_FAILURE;
    }
    pp_wait_completions(&ctx, 1);
    ctx.size = size;
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////

bool client_set_eager(void *kv_handle, const char *key, const char *value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    // send eager message
    char* flag = "se";
    sprintf(ctx.buf, "%s:%s:%s%c", flag, key, value, '\0');

    // send on the wire
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

bool client_set_rendezvous(void *kv_handle, const char *key, const char *value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;

    // 1. send malloc size, Format: "sr:key:valueSize\0"
    char* msg_size = (char*)malloc(10);
    sprintf(msg_size, "%d", strlen(value));
    char* flag = "sr";
    sprintf(ctx.buf, "%s:%s:%s%c", flag, key, msg_size, '\0');
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // 2. register memory
    struct ibv_mr* mr = ibv_reg_mr(ctx.pd, (void*) value, strlen(value), IBV_ACCESS_LOCAL_WRITE);
    if (!mr){
        fprintf(stderr, "Couldn't register memory region\n");
        return EXIT_FAILURE;
    }

    // 3. send memory region, Format: "rkey:addr\0"
    sprintf(ctx.buf, "%u:%lu", mr->rkey, (u_int64_t) mr->addr);
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // 4. Wait for ACK
    pp_wait_completions(&ctx, 1);

    // 5. Deregister memory
    if (ibv_dereg_mr(mr)){
        fprintf(stderr, "Couldn't deregister memory region\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////

bool client_get_rendezvous(void *kv_handle, const char *key, char **value, int size){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;

    // 1. malloc a buffer of size value_size
    char* value_buf = (char*)malloc(size + 1);
    if (value_buf == NULL){
        return EXIT_FAILURE;
    }
    value_buf[size] = '\0';

    // 2. register the buffer
    struct ibv_mr* mr = ibv_reg_mr(ctx.pd, value_buf, strlen(value_buf), IBV_ACCESS_LOCAL_WRITE |
                                                                            IBV_ACCESS_REMOTE_WRITE);
    if (!mr){
        fprintf(stderr, "Couldn't register memory region\n");
        return EXIT_FAILURE;
    }

    // 3. Receive msg from server. Format: "rkey:addr\0"
    uint64_t r_addr;
    uint32_t rkey;
    pp_wait_completions(&ctx, 1);
    sscanf(ctx.buf, "%u:%lu", &rkey, &r_addr);

    // 4. Read the value from the server
    if (pp_post_rdma_send(&ctx, mr, r_addr, rkey) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    pp_wait_completions(&ctx, 1);

    // 5. Send ACK
    if (send_ACK(kv_handle) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // 6. De-register memory
    if (ibv_dereg_mr(mr)){
        fprintf(stderr, "Couldn't deregister memory region\n");
        return EXIT_FAILURE;
    }

    // 7. Copy the value to the value pointer
    *value = value_buf;
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////

int kv_open(char *servername, void **kv_handle){
    struct pingpong_context** ctx_p = (struct pingpong_context**)kv_handle;
    init_connection(servername, ctx_p);
    return 0;
}

int kv_set(void *kv_handle, const char *key, const char *value){
    // Flag = {get, set}
    if (strlen(value) < 4*KB-3){
        // send rendezvous control message
        client_set_eager(kv_handle, key, value);
    }
    else{
        client_set_rendezvous(kv_handle, key, value);
    }
    return EXIT_SUCCESS;
}

int kv_get(void *kv_handle, const char *key, char **value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    // 1. send get message. Format: "g0:key\0"
    char* flag = "g0";
    sprintf(ctx.buf, "%s:%s:%c", flag, key,'\0');
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // receive the value
    pp_wait_completions(&ctx, 1);
    char* buf = ctx.buf;
    if (buf[0] == 'e'){
        // copy without the flag in the form of "e:value"
        buf = buf + 2;
        *value = (char*)malloc(strlen(buf) + 1);
        sprintf(*value, "%s", buf);
        return EXIT_SUCCESS;
    }
    else{
        // Malloc size of the value. Format: "r:size"
        int size;
        sscanf(buf, "r:%d", &size);
        client_get_rendezvous(kv_handle, key, value, size);
    }
}

/* Called after get() on value pointer */
void kv_release(char *value){
    free(value);
}

/* Destroys the QP */
int kv_close(void *kv_handle){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    pp_close_ctx(&ctx);
    free(kv_handle);
    return EXIT_SUCCESS;
}