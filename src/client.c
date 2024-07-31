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

int kv_open(char *servername, void **kv_handle){
    struct pingpong_context** ctx_p = (struct pingpong_context**)kv_handle;
    init_connection(servername, ctx_p);
    return 0;
}

int kv_set(void *kv_handle, const char *key, const char *value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    // Flag = {get, set}
    char flag = 's';
    sprintf(ctx.buf, "%c:%s:%s%c", flag, key, value, '\0');

    // send on the wire
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int kv_get(void *kv_handle, const char *key, char **value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    // Flag = {get, set}
    char flag = 'g';
    sprintf(ctx.buf, "%c:%s:%c", flag, key,'\0');
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
        buf = buf + 2;
        // receive rendezvous control message
        int size = atoi(buf);

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