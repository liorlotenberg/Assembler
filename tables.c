#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "tables.h"
#include "errors.h"


void insert_to_symbol_table(SymbolNode **head, char *symbolname, int value, char *type){
    /* add node to linked list
    create new node with given data and links it as the first node */
    SymbolNode* new_node = (SymbolNode *) malloc(sizeof(SymbolNode));
    if (new_node == NULL) {
        log_error(ERROR_3);
        return;
    }
    
    new_node->symbolname = (char *) malloc(strlen(symbolname) + 1);
    new_node->value = value;
    new_node->type = (char *) malloc(strlen(type) + 1);

    if (new_node->symbolname == NULL || new_node->type == NULL) {
        log_error(ERROR_3);
        return;
    }
    
    strcpy(new_node->symbolname, symbolname);
    strcpy(new_node->type, type);
    
    /* add node to the start of the list */
    new_node->next = *head;
    *head = new_node;
}


void print_symbol_table(SymbolNode* head){
    /* print linked list */
    while(head != NULL){
        printf("synbol name: %s \t|", head->symbolname);
        printf("value: %d \t|", head->value);
        printf("Node data: %s \n", head->type);
        head = head->next;
    } 
    putchar('\n');
}

SymbolNode *search_symbol_table(SymbolNode* head, char *symbolname){
    /* if name in list return the data of the node else return NULL*/
   while (head != NULL){
        if (strcmp(head->symbolname, symbolname) == 0){
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void free_symbol_table(SymbolNode *head){
    /* free memory allocations of the list and each of its nodes */
    SymbolNode *current_node = head;
    SymbolNode *next_node;

    while (current_node != NULL) {
        next_node = current_node->next;

        if (current_node->symbolname != NULL) free(current_node->symbolname);
        if (current_node->type != NULL) free(current_node->type);
        free(current_node);

        current_node = next_node;
    }
}

void update_data_address(SymbolNode *head, int IC){
    /* updates the addresses of the data according to the IC */
    while (head != NULL){
        if (strcmp(head->type, "data") == 0){
            head->value += IC;
        }
        head = head->next;
    }
}
