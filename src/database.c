#include "database.h"

bool create_database(Database** db_p){
    Database* db = (Database*)malloc(sizeof(Database));
    if (db == NULL){
        return EXIT_FAILURE;
    }
    db->head = NULL;
    *db_p = db;
    return EXIT_SUCCESS;
}

bool set_item(Database* db, char* key, char* value){
    Item* item = (Item*)malloc(sizeof(Item));
    if (item == NULL){
        return EXIT_FAILURE;
    }
    item->key = key;
    item->value = value;
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL){
        return EXIT_FAILURE;
    }
    node->data = item;
    node->next = db->head;
    db->head = node;
    return EXIT_SUCCESS;
}

bool get_value(Database* db, char* key, char** value_p){
    Node* current = db->head;
    while (current != NULL){
        if (strcmp(current->data->key, key) == 0){
            *value_p = current->data->value;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;
}