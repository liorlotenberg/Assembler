#ifndef DATA_STRUCT_H 
#define DATA_STRUCT_H

typedef struct Node{
    char *name;
    char *data;
    struct Node* next;
} Node;


void insert_to_start(Node **head, char *name, char *data) ;
    /* add node to linked list
    create new node with given data and links it as the first node */


void print_node(Node* head);
    /* print linked list */
    

char *search_node(Node* head, char *name);
    /* if name in list return the data of the node else return NULL*/
   

void free_list(Node *head);
    /* free memory allocations of the list and each of its nodes */


#endif