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
    char *value;
    int size;
    bool is_large;
    struct ibv_mr *mr;
} Value;

typedef struct item {
    char key[4 * KB];
    Value *value;
    int num_in_set;
    int num_in_get;
    bool client_set[NUM_CLIENTS];
    bool client_get[NUM_CLIENTS];
} Item;

typedef struct node {
    Item *data;
    struct node *next;
} Node;

typedef struct database {
    Node *head;
} Database;



int get_num_in_set(Database *db, const char *key);

int get_num_in_get(Database *db, const char *key);

bool add_num_in_set(Database *db, const char *key);

bool add_num_in_get(Database *db, const char *key);

bool valid_set(Database *db, const char *key);

bool valid_get(Database *db, const char *key);

bool add_get_query(Database *db, const char *key, int client_idx);

bool add_set_query(Database *db, const char *key, int client_idx);

bool* get_get_query(Database *db, const char *key);

bool* get_set_query(Database *db, const char *key);

void empty_set_query(Database *db, const char *key);

void empty_get_query(Database *db, const char *key);

bool remove_num_in_set(Database *db, const char *key);

bool remove_num_in_get(Database *db, const char *key);

bool create_database(Database **db);

bool set_item(Database *db, const char *key, Value *value);

bool get_value(Database *db, const char *key, Value **value_p);


#endif //WORKSHOP_NETWORK_EX3_DATABASE_H
