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
bool get_num_in_set(Database* db, const char* key, int* num_in_set)
{
    Node* current = db->head;
    while (current != NULL)
    {
        if (strcmp(current->data->key, key) == 0)
        {
            *num_in_set = current->data->num_in_set;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;

}

bool get_num_in_get(Database *db, const char *key, int *num_in_get) {
    Node* current = db->head;
    while (current != NULL)
    {
        if (strcmp(current->data->key, key) == 0)
        {
            *num_in_get = current->data->num_in_get;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;

}

bool add_num_in_set(Database *db, const char *key)
{
    Node* current = db->head;
    while (current != NULL)
    {
        if (strcmp(current->data->key, key) == 0)
        {
            current->data->num_in_set++;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;
}

bool add_num_in_get(Database *db, const char *key)
{
    Node* current = db->head;
    while (current != NULL)
    {
        if (strcmp(current->data->key, key) == 0)
        {
            current->data->num_in_get++;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;
}

bool remove_num_in_set(Database *db, const char *key)
{
    Node* current = db->head;
    while (current != NULL)
    {
        if (strcmp(current->data->key, key) == 0)
        {
            current->data->num_in_set--;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;
}

bool remove_num_in_get(Database *db, const char *key)
{
    Node* current = db->head;
    while (current != NULL)
    {
        if (strcmp(current->data->key, key) == 0)
        {
            current->data->num_in_get--;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    return EXIT_FAILURE;
}

bool _add_item(Database* db, char* key, Value* value) {
    Item *item = (Item *) malloc(sizeof(Item));
    if (item == NULL) {
        return EXIT_FAILURE;
    }
    strcpy(item->key, key);
    item->value = value;
    Node *node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
        return EXIT_FAILURE;
    }
    node->data = item;
    node->next = db->head;
    db->head = node;
    return EXIT_SUCCESS;
}

bool dealloc_value(Value** value) {
    if ((*value)->is_large) {
        if (ibv_dereg_mr((*value)->mr) != 0) {
            return EXIT_FAILURE;
        }
    }
    free((*value)->value);
    free(*value);
    *value = NULL;
    return EXIT_SUCCESS;
}


bool set_item(Database* db, const char* key, Value* value){
    // first check if the key already exists
    Node* current = db->head;
    while (current != NULL){
        if (strcmp(current->data->key, key) == 0){
            // if the key exists, update the value
            if (dealloc_value(&(current->data->value)) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }
            current->data->value = value;
            return EXIT_SUCCESS;
        }
        current = current->next;
    }
    // if the key does not exist, add it
    return _add_item(db, key, value);
}


bool get_value(Database* db, const char* key, Value** value_p){
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