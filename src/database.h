#ifndef WORKSHOP_NETWORK_EX3_DATABASE_H
#define WORKSHOP_NETWORK_EX3_DATABASE_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "infiniband/verbs.h"
#include "bw_template.h"

#define KB 1024


typedef struct Value {
    char* value;
    int size;
    bool is_large;
    struct ibv_mr* mr;
} Value;


typedef struct value_node {
    Value* data;
    struct db_node* next;
} ValueNode;

typedef struct value_linkList {
    ValueNode* head;
} ValuesList;


typedef struct db_item {
    char key[4*KB];

    int num_reads;
    // link list of values for readers
    ValuesList* values;

    // array of size NUM_CLIENTS, each element is a pointer to a Value struct
    Value* progress[NUM_CLIENTS];
} DBItem;

typedef struct db_node {
    DBItem* data;
    struct db_node* next;
} DBNode;

typedef struct database {
    DBNode* head;
} Database;






bool create_database(Database** db);

bool set_item(Database* db, const char* key, Value* value);

bool get_value(Database* db, const char* key, Value** value_p);




#endif //WORKSHOP_NETWORK_EX3_DATABASE_H
