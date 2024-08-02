#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "bw_template.h"
#include <stdbool.h>
#include "database.h"

typedef struct kv_handle {
    struct pingpong_context ctx;
}kvHandle;


int kv_open(char *servername, void **kv_handle); /*Connect to server*/

int kv_set(void *kv_handle, const char *key, const char *value);

int kv_get(void *kv_handle, const char *key, char **value);

void kv_release(char *value);/* Called after get() on value pointer */

int kv_close(void *kv_handle); /* Destroys the QP */

////// Server functions

bool receive_query(Database* db, kvHandle* kv_handle);

#endif //_CLIENT_H_