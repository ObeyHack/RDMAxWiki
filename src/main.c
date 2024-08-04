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
    char* key2 = "key2";
    char* value2 = "value2";
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
    if (result == EXIT_FAILURE) {
        printf(RED "Test 3: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 3: Failed\n" BASE);
        return;
    }

    printf(GREEN "Test 3: Passed\n" BASE);
}

void test_4(char* servername){
    printf(BLUE "Test 4: Double Set and Get (same keys) in Eager mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 4: Failed\n" BASE);
        return;
    }

    // Set + Get 1
    char* key = "key";
    char* value = "value";
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 4: Failed\n" BASE);
        return;
    }

    // Set + Get 2
    char* key2 = "key";
    char* value2 = "value2";
    result = set_get(kv_handle, key2, value2);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 4: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 4: Failed\n" BASE);
        return;
    }

    printf(GREEN "Test 4: Passed\n" BASE);
}

void test_5(char* servername) {
    printf(BLUE "Test 5: Set and Get in Rendezvous mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 5: Failed\n" BASE);
        return;
    }

    // Set + Get
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';

    result = set_get(kv_handle, key, value);

    if (result == EXIT_FAILURE) {
        printf(RED "Test 5: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 5: Failed\n" BASE);
        return;
    }

    //deallocating memory
    free(value);
    printf(GREEN "Test 5: Passed\n" BASE);
}

void test_6(char* servername){
    printf(BLUE "Test 6: Double Set and Get (different keys) in Rendezvous mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 6: Failed\n" BASE);
        return;
    }

    // Set + Get 1
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 6: Failed\n" BASE);
        return;
    }

    // Set + Get 2
    char* key2 = "key2";
    char* value2 = (char*)malloc(4*KB+10);
    memset(value2, 'b', 4*KB+10);
    result = set_get(kv_handle, key2, value2);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 6: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 6: Failed\n" BASE);
        return;
    }


    //deallocating memory
    free(value);
    free(value2);
    printf(GREEN "Test 6: Passed\n" BASE);
}

void test_7(char* servername){
    printf(BLUE "Test 7: Double Set and Get (same keys) in Rendezvous mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 7: Failed\n" BASE);
        return;
    }

    // Set + Get 1
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 7: Failed\n" BASE);
        return;
    }

    // Set + Get 2
    char* value2 = (char*)malloc(4*KB+10);
    memset(value2, 'b', 4*KB+10);
    result = set_get(kv_handle, key, value2);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 7: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 7: Failed\n" BASE);
        return;
    }


    //deallocating memory
    free(value);
    free(value2);
    printf(GREEN "Test 7: Passed\n" BASE);
}

void test_8(char* servername){
    printf(BLUE "Test 8: Double Set and Get Eager then Rendezvous mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 8: Failed\n" BASE);
        return;
    }

    // Set + Get 1
    char* key = "key";
    char* value = "value";
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 8: Failed\n" BASE);
        return;
    }

    // Set + Get 2
    char* value2 = (char*)malloc(4*KB+10);
    memset(value2, 'a', 4*KB+10);
    result = set_get(kv_handle, key, value2);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 8: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 8: Failed\n" BASE);
        return;
    }


    //deallocating memory
    free(value2);
    printf(GREEN "Test 8: Passed\n" BASE);
}

void test_9(char* servername){
    printf(BLUE "Test 9: Double Set and Get Rendezvous then Eager mode\n" BASE);

    // open connection
    kvHandle* kv_handle;
    bool result = kv_open(servername, (void **) &kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 9: Failed\n" BASE);
        return;
    }

    // Set + Get 1
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';
    result = set_get(kv_handle, key, value);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 9: Failed\n" BASE);
        return;
    }

    // Set + Get 2
    char* value2 = "value2";
    result = set_get(kv_handle, key, value2);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 9: Failed\n" BASE);
        return;
    }

    // close connection
    result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 9: Failed\n" BASE);
        return;
    }

    //deallocating memory
    free(value);
    printf(GREEN "Test 9: Passed\n" BASE);
}

void run_tests(char* servername){
    printf(GOLD "Running tests\n" BASE);
//    test_1(servername);
//    test_2(servername);
//    test_3(servername);
//    test_4(servername);
//    test_5(servername);
//    test_6(servername);
//    test_7(servername);
//    test_8(servername);
//    test_9(servername);
}

bool execute_query(kvHandle* kv_handle, char* line){
    /*
     * Parse the line and execute the query. Format:
     * se:key:value
     * sr:key:valueSize
     * ge:key:x
     */
    char* flag;
    char* key;
    char* value;
    sscanf(line, "%s:%s:%s", flag, key, value);


}

void parse_input(char* servername, char* input_file){
    /*
     * Parse the input file and execute the queries.
     */
    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        printf(RED "Error opening file\n" BASE);
        return;
    }
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    // open connection
    kvHandle* kv_handle;
    kv_open("localhost", (void**)&kv_handle);

    while ((read = getline(&line, &len, file)) != -1) {
        if (execute_query(kv_handle, line) == EXIT_FAILURE) {
            printf(RED "Error executing query\n" BASE);
            return;
        }
    }

    // close connection
    kv_close(kv_handle);
}

int main(int argc, char *argv[]) {
    char* servername = argv[1];
    kvHandle* kv_handle;

    if (argc == 2) {
        run_tests(servername);
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
