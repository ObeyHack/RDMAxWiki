#include "client.h"
#include "bw_template.h"
#include "database.h"
#include <stdbool.h>
#include <stdlib.h>


bool parse_data(Database* db, kvHandle* kv_handle, char* buf){
    // Data format: flag:key:value
    char* flag = strtok(buf, ":");

    if (strlen(flag) != 1){
        return EXIT_FAILURE;
    }
    if (strcmp(flag, "s") != 0 && strcmp(flag, "g") != 0){
        return EXIT_FAILURE;
    }

    if (strcmp(flag, "s")==0){
        char* key = strtok(NULL, ":");
        char* value = strtok(NULL, ":");
        if (set_item(db, key, value) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        send_ACK(kv_handle);
    }
    else{
        char* key = strtok(NULL, ":");
        char* value;
        if (get_value(db, key, &value) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        // turn value into a string
        value = strtok(value, "\0");
        send_data_str(kv_handle, value);
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
        if (kv_set(kv_handle, "key", "value") == EXIT_FAILURE) {
            printf("Failed to set key\n");
            return EXIT_FAILURE;
        }
        char* value;
        if (kv_get(kv_handle, "key", &value) == EXIT_FAILURE) {
            printf("Failed to get key\n");
            return EXIT_FAILURE;
        }
        printf("Value: %s\n", value);
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
