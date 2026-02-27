#ifndef TABLES_H 
#define TABLES_H

typedef struct SymbolNode{
    char *symbolname;
    int value;
    char *type;
    struct SymbolNode* next;
} SymbolNode;


void insert_to_symbol_table(SymbolNode **head, char *symbolname, int value, char *type);
    /* add node to linked list
    create new node with given data and links it as the first node */


void print_symbol_table(SymbolNode* head);
    /* print linked list */
    

SymbolNode *search_symbol_table(SymbolNode* head, char *symbolname);
    /* if name in list return the data of the node else return NULL*/
   

void free_symbol_table(SymbolNode *head);
    /* free memory allocations of the list and each of its nodes */

void update_data_address(SymbolNode *head, int IC);
    /* updates the addresses of the data according to the IC */

#endif