#include "client.h"
#include "database.h"
#include <stdbool.h>
#include <stdlib.h>

#define BLUE "\x1B[34m"
#define BASE "\x1B[0m"
#define GOLD "\x1B[33m"
#define GREEN "\x1B[32m"
#define RED "\x1B[31m"

bool set_get(kvHandle* kv_handle, char* key, char* value){
    bool result = kv_set(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    char* value_retrieved;
    result = kv_get(kv_handle, key, &value_retrieved);
    if (result == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    if (strcmp(value, value_retrieved) != 0) {
        return EXIT_FAILURE;
    }

    kv_release(value_retrieved);
    return EXIT_SUCCESS;
}


void test_1(char* servername){
    printf(BLUE "Test 1: Double connection and disconnection\n" BASE);

    for (int i = 0; i < 2; i++) {
        kvHandle* kv_handle;
        bool result = kv_open(servername, (void **) &kv_handle);
        if (result == EXIT_FAILURE) {
            printf(RED "Test 1: Failed\n" BASE);
            return;
        }

        result = kv_close(kv_handle);
        if (result == EXIT_FAILURE) {
            printf(RED "Test 1: Failed\n" BASE);
            return;
        }
    }
    printf(GREEN "Test 1: Passed\n" BASE);
}

void test_2(char* servername){
    printf(BLUE "Test 2: Set and Get in Eager mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 2: Failed\n" BASE);
        return;
    }

    // Set + Get
    char* key = "key";
    char* value = "value";
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 2: Failed\n" BASE);
        return;
    }

    // Set + Get
    char* key = "key";
    char* value = "value";
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 2: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 2: Failed\n" BASE);
        return;
    }

    printf(GREEN "Test 2: Passed\n" BASE);
}

void test_3(char* servername){
    printf(BLUE "Test 3: Double Set and Get (different keys) in Eager mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 3: Failed\n" BASE);
        return;
    }

    // Set + Get 1
    char* key = "key";
    char* value = "value";
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 3: Failed\n" BASE);
        return;
    }

    // Set + Get 2
    char* key2 = "key2";
    char* value2 = "value2";
    result = set_get(kv_handle, key2, value2);

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 3: Failed\n" BASE);
        return;
    }

    printf(GREEN "Test 3: Passed\n" BASE);
}




int main(int argc, char *argv[]) {
    char* servername = argv[1];
    kvHandle* kv_handle;

    if (argc == 2) {
        //test_1(servername);
        test_3(servername);
//        printf("Connected to server %s\n", servername);
//        kv_open(servername, (void **) &kv_handle);
//        // msg bigger than 4KB with "\0" at the end
//        char* value = (char*)malloc(4*KB+10);
//        memset(value, 'a', 4*KB+10);
//        value[4*KB+10] = '\0';
//        if (kv_set(kv_handle, "key", value) == EXIT_FAILURE) {
//            printf("Failed to set key\n");
//            return EXIT_FAILURE;
//        }
//        char* value_retrieved;
//        if (kv_get(kv_handle, "key", &value_retrieved) == EXIT_FAILURE) {
//            printf("Failed to get key\n");
//            return EXIT_FAILURE;
//        }
//        printf("Value: %s\n", value_retrieved);
    }

    else if (argc == 1) {
        Database* db;
        create_database(&db);
        printf(GOLD "Server started\n" BASE);
        kv_open(servername, (void**)&kv_handle);
        while (1){
            receive_query(db, kv_handle);
        }
    }

    return EXIT_SUCCESS;
}
