#ifndef PRE_ASSEMBLER_H 
#define PRE_ASSEMBLER_H

#include "data_struct.h"


void save_macro(char *macro_name, char *macro_data, Node **macros_list);
    /* add macro to linked list */


int check_valid_macro_declared(char declare_line[]);
    /* gets the first line of macro declaration. returns 1 if macro declared right and 0 if not.
    1. there arent any unnecessary chars in the line.
    2. the name of the macro isn't an instruction or other saved word.
    */


int check_valid_macro_declare_end(char end_line[]);
    /* return 1 if macro declared right and 0 if not.
        1. there arent any unnecessary chard in the line.
    */


int replace_macros(char filename[], Node **macros_list);
    /* go through an .as file, search for macros
    if a macro is defined -> save it into XXX and 'delete' those line
    if a macro is called -> 'replace' it with the macro data*/


#endif