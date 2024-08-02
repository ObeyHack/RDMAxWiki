#ifndef WORKSHOP_NETWORK_EX3_DATABASE_H
#define WORKSHOP_NETWORK_EX3_DATABASE_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "infiniband/verbs.h"

#define KB 1024

typedef struct Value {
    char* value;
    int size;
    bool is_large;
    struct ibv_mr* mr;
} Value;

typedef struct item {
    char key[4*KB];
    Value* value;
} Item;

typedef struct node {
    Item* data;
    struct node* next;
} Node;

typedef struct database {
    Node* head;
} Database;



bool create_database(Database** db);

bool set_item(Database* db, const char* key, Value* value);

bool get_value(Database* db, const char* key, Value** value_p);




#endif //WORKSHOP_NETWORK_EX3_DATABASE_H
