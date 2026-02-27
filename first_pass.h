#ifndef FIRST_PASS_H 
#define FIRST_PASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tables.h"
#include "errors.h"
#include "utils.h"
#include "data_struct.h"


int process_string(char *line_without_label, char *filename, int line_num, unsigned short data_image[], int *DC);
    /* read the data, covert it to ascii, add to data image, increase DC*/

int process_data(char *line_without_label, char *filename, int line_num, unsigned short data_image[], int *DC);
    /* read the numbers, convert into 12 bits, add to data image, increase DC*/

int process_extern(char *line_without_label, char *filename, int line_num, SymbolNode **symbols_table);
    /* check if the extern label is valid, and add it to symbol table */

int process_command(char *line_without_label, char *filename, int line_num, unsigned short code_image[], int *IC);
    /* gets a command line and translte it into machine language,
    during the translate it check if good amount of operands passed, 
    are them written correctly, and if the addresing mode is legal */

int first_pass(char * filename, Node *macros_list);
    /* execute first pass */
       

#endif