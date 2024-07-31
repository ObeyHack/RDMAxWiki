#include "client.h"
#include "bw_template.h"
#include "database.h"
#include <stdbool.h>
#include <stdlib.h>

bool server_get_rendezvous(Database* db, kvHandle* kv_handle, char* value){
    // value is bigger than 4KB
    // send rendezvous control message: "r:{size of value}"
    char* msg = (char*)malloc((int)strlen(value) + 3);
    if (msg == NULL){
        return EXIT_FAILURE;
    }
    sprintf(msg, "r:%d", (int)strlen(value));
    send_data_str(kv_handle, msg);
}

bool server_set_rendezvous(Database* db, kvHandle* kv_handle, char* key, int value_size){
    // value is bigger than 4KB
    // send rendezvous control message: "r:{size of value}"
    char* value = (char*)malloc(value_size + 1);
    memset(value, 'a', value_size);
    value[value_size] = '\0';
    if (set_item(db, key, value) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    // Exchange memory regions
}

bool parse_data(Database* db, kvHandle* kv_handle, char* buf){
    // Data format: flag:key:value
    char* flag = strtok(buf, ":");

    if (strlen(flag) != 2){
        return EXIT_FAILURE;
    }
    if (strcmp(flag, "se") != 0 && strcmp(flag, "sr") != 0 && strcmp(flag, "g0") != 0){
        return EXIT_FAILURE;
    }

    if (strcmp(flag, "se")==0){
        char* key = strtok(NULL, ":");
        char* value = strtok(NULL, ":");
        if (set_item(db, key, value) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
    }

    else if (strcmp(flag, "sr")==0){
        char* key = strtok(NULL, ":");
        char* size = strtok(NULL, ":");
        int size_int = atoi(size);

        if (server_set_rendezvous(db, kv_handle, key, size_int) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
    }

    else{
        char* key = strtok(NULL, ":");
        char* value;
        if (get_value(db, key, &value) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        // turn value into a string
        value = strtok(value, "\0");
        if (strlen(value) < 4*KB-3){
            // add a flag to the value in the form of "e:value"
            //TODO: extra malloc
            char* new_value = (char*)malloc(strlen(value) + 3);
            if (new_value == NULL){
                return EXIT_FAILURE;
            }
            sprintf(new_value, "e:%s", value);
            send_data_str(kv_handle, new_value);
            free(new_value);
        }
        else{
            // send rendezvous
            if (server_get_rendezvous(db, kv_handle, value) == EXIT_FAILURE){
                return EXIT_FAILURE;
            }
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
