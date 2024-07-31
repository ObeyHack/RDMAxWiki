#include "client.h"


int kv_open(char *servername, void **kv_handle){
    struct pingpong_context** ctx_p = (struct pingpong_context**)kv_handle;
    init_connection(servername, ctx_p);
    return 0;
}

int kv_set(void *kv_handle, const char *key, const char *value){
    return 0;
}

int kv_get(void *kv_handle, const char *key, char **value){
    return 0;
}

/* Called after get() on value pointer */
void kv_release(char *value){
    return;
}

/* Destroys the QP */
int kv_close(void *kv_handle){
    return 0;
}