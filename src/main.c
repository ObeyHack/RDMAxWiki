#include "client.h"
#include "bw_template.h"
#include "database.h"
#include <stdbool.h>
#include <stdlib.h>

//////////////////////// Get Server  ////////////////////////

bool server_get_rendezvous(Database* db, kvHandle* kv_handle, Value* value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;

    // 1. send malloc size, Format: "r:valueSize\0"
    sprintf(ctx.buf, "r:%d", value->size);
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // 2. send rkey and raddr. Format: rkey:raddr\0
    sprintf(ctx.buf, "%u:%lu", value->mr->rkey, (u_int64_t)value->mr->addr);
    if (send_data_str(kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // 3. Wait for ACK
    pp_wait_completions(&ctx, 1);
    return EXIT_SUCCESS;
}

bool server_get_eager(kvHandle* kv_handle, Value* value){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    sprintf(ctx.buf, "e:%s", value->value);
    send_data_str(kv_handle, ctx.buf);
    return EXIT_SUCCESS;
}


//////////////////////// Set Server  ////////////////////////

bool server_set_rendezvous(Database* db, kvHandle* kv_handle, char* key, int value_size){
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;

    // 1. malloc a buffer of size value_size
    char* value = (char*)malloc(value_size + 1);
    value[value_size] = '\0';

    // 2. register the buffer
    struct ibv_mr* mr = ibv_reg_mr(ctx.pd, (void*) value, strlen(value), IBV_ACCESS_LOCAL_WRITE |
                                                                            IBV_ACCESS_REMOTE_READ |
                                                                            IBV_ACCESS_REMOTE_WRITE);
    if (!mr){
        fprintf(stderr, "Couldn't register memory region\n");
        return EXIT_FAILURE;
    }

    // 3. Receive msg from client. Format: "rkey:addr\0"
    uint64_t r_addr;
    uint32_t rkey;
    pp_wait_completions(&ctx, 1);
    sscanf(ctx.buf, "%u:%lu", &rkey, &r_addr);

    // 4. Read the value from the client
    if (pp_post_rdma_send(&ctx, mr, r_addr, rkey) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    pp_wait_completions(&ctx, 1);

    // 5. Set the item in the database
    Value *value_struct = (Value *) malloc(sizeof(Value));
    if (value_struct == NULL) {
        return EXIT_FAILURE;
    }
    value_struct->value = value;
    value_struct->size = value_size;
    value_struct->is_large = true;
    value_struct->mr = mr;
    if (set_item(db, key, value_struct) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // 6. Send ACK
    if (send_ACK(kv_handle) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


bool server_set_eager(Database* db, kvHandle* kv_handle, char* key, char* value){
    Value *value_struct = (Value *) malloc(sizeof(Value));
    if (value_struct == NULL) {
        return EXIT_FAILURE;
    }
    value_struct->value = value;
    value_struct->size = strlen(value);
    value_struct->is_large = false;
    value_struct->mr = NULL;
    if (set_item(db, key, value_struct) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


//////////////////////// Receive Query  ////////////////////////

bool parse_data(Database* db, kvHandle* kv_handle, char* buf){
    // Data format: flag:key:x
    char* flag = strtok(buf, ":");

    if (strlen(flag) != 2){
        return EXIT_FAILURE;
    }
    if (strcmp(flag, "se") != 0 && strcmp(flag, "sr") != 0 && strcmp(flag, "g0") != 0){
        return EXIT_FAILURE;
    }
    char key[4*KB];
    strcpy(key, strtok(NULL, ":"));

    //////////////////////// Set Eager ////////////////////////
    if (strcmp(flag, "se")==0){
        char* value = strtok(NULL, ":");
        char* copy = (char*)malloc(strlen(value) + 1);
        if (copy == NULL){
            return EXIT_FAILURE;
        }
        strcpy(copy, value);
        if (server_set_eager(db, kv_handle, key, copy) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    //////////////////////// Set Rendezvous ////////////////////////
    else if (strcmp(flag, "sr")==0){
        char* size = strtok(NULL, ":");
        int size_int = atoi(size);
        if (server_set_rendezvous(db, kv_handle, key, size_int) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    Value* value;
    if (get_value(db, key, &value) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    //////////////////////// Get Eager ////////////////////////
    if (value->is_large == false){
        if (server_get_eager(kv_handle, value) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
    }

    //////////////////////// Get Rendezvous ////////////////////////
    else{
        if (server_get_rendezvous(db, kv_handle, value) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}



bool receive_query(Database* db, kvHandle* kv_handle){
    // Receive data
    struct pingpong_context ctx = *(struct pingpong_context*)kv_handle;
    pp_wait_completions(&ctx, 1);

    // Parse data
    if (parse_data(db, kv_handle, ctx.buf) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
    char* servername = argv[1];
    kvHandle* kv_handle;

    if (argc > 1) {
        printf("Connected to server %s\n", servername);
        kv_open(servername, (void **) &kv_handle);
        // msg bigger than 4KB with "\0" at the end
        char* value = (char*)malloc(4*KB+10);
        memset(value, 'a', 4*KB+10);
        value[4*KB+10] = '\0';
        if (kv_set(kv_handle, "key", value) == EXIT_FAILURE) {
            printf("Failed to set key\n");
            return EXIT_FAILURE;
        }
        char* value_retrieved;
        if (kv_get(kv_handle, "key", &value_retrieved) == EXIT_FAILURE) {
            printf("Failed to get key\n");
            return EXIT_FAILURE;
        }
        printf("Value: %s\n", value_retrieved);
    }

    else{
        Database* db;
        create_database(&db);
        printf("Server started\n");
        kv_open(servername, (void**)&kv_handle);
        while (1){
            receive_query(db, kv_handle);
        }
    }

    return EXIT_SUCCESS;
}
