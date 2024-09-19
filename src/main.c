#include "client.h"
#include "database.h"
#include <stdbool.h>
#include <stdlib.h>

#define BLUE "\x1B[34m"
#define BASE "\x1B[0m"
#define GOLD "\x1B[33m"
#define GREEN "\x1B[32m"
#define RED "\x1B[31m"
#define PURPLE "\x1B[35m"

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

void test_double_client(char* servername){
    printf(BLUE "Test 1: Double connection and disconnection\n" BASE);

    for (int i = 0; i < 1; i++) {
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

void test_connection(char* servername, kvHandle** kv_handle_ptr){
    printf(BLUE "Test: Connection\n" BASE);

    bool result = kv_open(servername, (void **) kv_handle_ptr);
    if (result == EXIT_FAILURE) {
        printf(RED "Test 1: Failed\n" BASE);
        return;
    }


    printf(GREEN "Test 1: Passed\n" BASE);
}

void test_disconnection(kvHandle* kv_handle){
    printf(BLUE "Test: Disconnection\n" BASE);

    bool result = kv_close(kv_handle);
    if (result == EXIT_FAILURE) {
        printf(RED "Test: Failed\n" BASE);
        return;
    }

    printf(GREEN "Test: Passed\n" BASE);
}

void test_2(kvHandle* kv_handle){
    printf(BLUE "Test 2: Set and Get in Eager mode\n" BASE);

    // Set + Get
    char* key = "key";
    char* value = "value";
    bool result = set_get(kv_handle, key, value);
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

    printf(GREEN "Test 2: Passed\n" BASE);
}

void test_3(kvHandle* kv_handle){
    printf(BLUE "Test 3: Double Set and Get (different keys) in Eager mode\n" BASE);

    // Set + Get 1
    char* key = "key";
    char* value = "value";
    bool result = set_get(kv_handle, key, value);
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

    printf(GREEN "Test 3: Passed\n" BASE);
}

void test_4(kvHandle* kv_handle){
    printf(BLUE "Test 4: Double Set and Get (same keys) in Eager mode\n" BASE);


    // Set + Get 1
    char* key = "key";
    char* value = "value";
    bool result = set_get(kv_handle, key, value);
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

    printf(GREEN "Test 4: Passed\n" BASE);
}

void test_5(kvHandle* kv_handle) {
    printf(BLUE "Test 5: Set and Get in Rendezvous mode\n" BASE);

    // Set + Get
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';

    bool result = set_get(kv_handle, key, value);

    if (result == EXIT_FAILURE) {
        printf(RED "Test 5: Failed\n" BASE);
        return;
    }

    //deallocating memory
    free(value);
    printf(GREEN "Test 5: Passed\n" BASE);
}

void test_6(kvHandle* kv_handle){
    printf(BLUE "Test 6: Double Set and Get (different keys) in Rendezvous mode\n" BASE);

    // Set + Get 1
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';
    bool result = set_get(kv_handle, key, value);
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

    //deallocating memory
    free(value);
    free(value2);
    printf(GREEN "Test 6: Passed\n" BASE);
}

void test_7(kvHandle* kv_handle){
    printf(BLUE "Test 7: Double Set and Get (same keys) in Rendezvous mode\n" BASE);

    // Set + Get 1
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';
    bool result = set_get(kv_handle, key, value);
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

    //deallocating memory
    free(value);
    free(value2);
    printf(GREEN "Test 7: Passed\n" BASE);
}

void test_8(kvHandle* kv_handle){
    printf(BLUE "Test 8: Double Set and Get Eager then Rendezvous mode\n" BASE);

    // Set + Get 1
    char* key = "key";
    char* value = "value";
    bool result = set_get(kv_handle, key, value);
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

    //deallocating memory
    free(value2);
    printf(GREEN "Test 8: Passed\n" BASE);
}

void test_9(kvHandle* kv_handle){
    printf(BLUE "Test 9: Double Set and Get Rendezvous then Eager mode\n" BASE);

    // Set + Get 1
    char* key = "key";
    char* value = (char*)malloc(4*KB+10);
    memset(value, 'a', 4*KB+10);
    value[4*KB+10] = '\0';
    bool result = set_get(kv_handle, key, value);
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

    //deallocating memory
    free(value);
    printf(GREEN "Test 9: Passed\n" BASE);
}

int send_data_kv_set(kvHandle* kv_handle, int iters, const char *key, char *value){
    // set data size to send in ctx
    int i;
    for (i = 0; i < iters; i++) {
        if (kv_set(kv_handle, key, value) != 0){
            fprintf(stderr, "Client couldn't post send\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}


void test_throughout(kvHandle* kv_handle){
    printf(PURPLE "Test throughout: Set Eager mode\n" BASE);

    // Set
    char* key = "key";
    char* value;
    double* throughputs = (double*) malloc((MEGA_POWER+1) * sizeof(double));
    int index = 0;
    struct timeval start, end;
    for (int i = 1; i <= MEGABIT; i <<= 1) {
        value = (char*)malloc(i);
        memset(value, 'a', i);
        value[i] = '\0';
        // warm up
        send_data_kv_set(kv_handle,  WARMUP_CYCLES, key, value);
        // measure
        gettimeofday(&start, NULL);
        send_data_kv_set(kv_handle, MSG_COUNT, key, value);
        gettimeofday(&end, NULL);
        throughputs[index] = calc_throughput(start, end, i);
        // print throughput
        printf("%d\t%f\tMbit/s\n", i, throughputs[index]);
        free(value);
        index++;
    }

}

void run_tests(char* servername){
    printf(GOLD "Running tests\n" BASE);
    kvHandle *kv_handle;
    test_connection(servername, &kv_handle);
    sleep(8);

//    test_throughout(kv_handle);
    test_2(kv_handle);
//    sleep(2);
    test_3(kv_handle);
//    sleep(2);
    test_4(kv_handle);
//    sleep(2);
    test_5(kv_handle);
//    sleep(2);
    test_6(kv_handle);
//    sleep(2);
    test_7(kv_handle);
//    sleep(2);
    test_8(kv_handle);
//    sleep(2);
    test_9(kv_handle);

    sleep(8);
    test_disconnection(kv_handle);
}


bool execute_query(kvHandle *kv_handle, char *line) {
    /*
     * Parse the line and execute the query. Format:
     * se:key:value
     * sr:key:valueSize
     * ge:key:x
     */
    char flag[3];
    char key[256];
    char value[256];
    sscanf(line, "%2[^:]:%255[^:]:%255[^\n]", flag, key, value);// bug here
    if (flag == NULL || key == NULL || value == NULL) {
        return EXIT_FAILURE;
    }
    if (flag[0] == 's' && flag[1] == 'e') {
        return kv_set(kv_handle, key, value);
    } else if (flag[0] == 's' && flag[1] == 'r') {
        int value_size;
        sscanf(line, "%s:%s:%d", flag, key, &value_size);
        char *value_sr = (char *) malloc(value_size);
        memset(value_sr, 'a', value_size);
        value_sr[value_size] = '\0';
        bool result = kv_set(kv_handle, key, value_sr);
        free(value_sr);
        return result;

    } else if (flag[0] == 'g' && flag[1] == 'e') {
        char *value_retrieved;
        return kv_get(kv_handle, key, &value_retrieved);
    }
    return EXIT_FAILURE;

}

void parse_input(char* servername, char *input_file) {
    /*
     * Parse the input file and execute the queries.
     */
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf(RED "Error opening file\n" BASE);
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // open connection
    kvHandle *kv_handle;
    kv_open(servername, (void **) &kv_handle);// here should be servername

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

    if (argc == 3) {
        parse_input(servername, argv[2]);
    }

    if (argc == 2) {
        run_tests(servername);
    }

    else if (argc == 1) {
        Database* db;
        create_database(&db);
        printf(GOLD "Server started\n" BASE);

        // num of client connections
        kvHandle* kv_handles[NUM_CLIENTS];

        for (int i = 0; i < NUM_CLIENTS; i++) {
            kv_open(servername, (void**)&kv_handles[i]);
        }

        while (1){
            receive_query(db, kv_handles);
        }
    }

    return EXIT_SUCCESS;
}
