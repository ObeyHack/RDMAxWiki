#include "client.h"
#include "bw_template.h"
#include "database.h"




int main(int argc, char *argv[]) {
    char* servername = argv[1];
    void* kv_handle;
    kv_open(servername, &kv_handle);

    if (servername)
        printf("Connected to server %s\n", servername);

    else{
        printf("Server started\n");
        Database* db;
        create_database(&db);
    }
}
