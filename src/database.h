#ifndef WORKSHOP_NETWORK_EX3_DATABASE_H
#define WORKSHOP_NETWORK_EX3_DATABASE_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct item {
    char* key;
    char* value;
} Item;

typedef struct node {
    Item* data;
    struct node* next;
} Node;

typedef struct database {
    Node* head;
} Database;



bool create_database(Database** db_p);

bool set_item(Database* db, char* key, char* value);

bool get_value(Database* db, char* key, char** value_p);




#endif //WORKSHOP_NETWORK_EX3_DATABASE_H
