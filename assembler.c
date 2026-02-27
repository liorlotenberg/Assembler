#include <stdio.h>
#include <stdlib.h>
#include "pre_assembler.h"
#include "first_pass.h"
#include "data_struct.h"

int main(int argc, char* argv[]){
    int i;
    Node *macros_list = NULL;
    for (i = 1; i < argc; i++){
        macros_list = NULL;
        /* execute pre-processor */
        if (!replace_macros(argv[i], &macros_list)){
            /* execute frist pass */
            first_pass(argv[i], macros_list);
        };
        free_list(macros_list);
    } 
    return 0;
}