#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"
#include "errors.h"

void insert_to_start(Node **head, char *name, char *data) {
    /* add node to linked list */
    /* create new node with given data */
    Node* new_node = (Node *) malloc(sizeof(Node));
    if (new_node == NULL) {
        log_error(ERROR_3);
        return;
    }
    
    new_node->name = (char *) malloc(strlen(name) + 1);
    new_node->data = (char *) malloc(strlen(data) + 1);

    if (new_node->name == NULL || new_node->data == NULL) {
        log_error(ERROR_3);
        return;
    }
    
    strcpy(new_node->name, name);
    strcpy(new_node->data, data);
    
    /* add node to the start of the list */
    new_node->next = *head;
    *head = new_node;
}


void print_node(Node* head){
    /* print linked list */
    while(head != NULL){
        printf("Node name: %s ", head->name);
        printf("Node data: %s ", head->data);
        head = head->next;
    } 
    putchar('\n');
}

char *search_node(Node* head, char *name){
    /* if name in list return the data of the node else return NULL*/
    while (head != NULL){
        if (strcmp(head->name, name) == 0){
            return head->data;
        }
        head = head->next;
    }
    return NULL;
}


void free_list(Node *head) {
    /* free memory allocations */
    Node *current_node = head;
    Node *next_node;

    while (current_node != NULL) {
        next_node = current_node->next;

        if (current_node->name != NULL) free(current_node->name);
        if (current_node->data != NULL) free(current_node->data);
        free(current_node);

        current_node = next_node;
    }
}
