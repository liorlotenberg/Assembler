#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "data_struct.h"
#include "pre_assembler.h"
#include "errors.h"

void save_macro(char *macro_name, char *macro_data, Node **macros_list){
    /* add macro to linked list */
    insert_to_start(macros_list, macro_name, macro_data);
}


int check_valid_macro_declared(char declare_line[]){
    /* gets the first line of macro declaration. returns 1 if macro declared right and 0 if not.
    1. there arent any unnecessary chars in the line.
    2. the name of the macro isn't an instruction or other saved word.
    */
    int is_valid = 1;
    int count;
    char macro_name[MAX_LINE];
    char tmp_str1[MAX_LINE];
    char tmp_str2[MAX_LINE];
    int i = 0;

    count = sscanf(declare_line,"%s %s %s", tmp_str1, macro_name, tmp_str2);
    /* check if there are any unnecessary chars in line */
    if (count == 2){
        /* check if not saved word*/
        for (i = 0; i < INSTR_COUNT; i++){
            if (strcmp(macro_name, INSTRUCTIONS[i]) == 0) {
                is_valid = 0;
            }
        }
        for (i = 0; i < REGS_COUNT; i++){
            if (strcmp(macro_name, REGS[i]) == 0) {
                is_valid = 0;
            }
        }   
        for (i = 0; i < OP_CODES_COUNT; i++){
            if (strcmp(macro_name, OP_CODES[i]) == 0) {
                is_valid = 0;
            }
        }
    }
    else{
        is_valid = 0;
    }
    return is_valid;
}

int check_valid_macro_declare_end(char end_line[]){
    /* return 1 if macro declared right and 0 if not.
        1. there arent any unnecessary chard in the line.
    */
    int is_valid = 1;
    char tmp_str1[MAX_LINE];
    char tmp_str2[MAX_LINE];
    int count = sscanf(end_line,"%s %s", tmp_str1, tmp_str2);

    /* check if there are any unnecessary chars in line */
    if (count != 1){
        is_valid = 0;
    }
    return is_valid;
}

int replace_macros(char filename[], Node **macros_list){
    /* go through an .as file, search for macros
    if a macro is defined -> save it into the macros list and 'delete' those line
    if a macro is called -> 'replace' it with the macro data*/
    FILE *input_file = NULL;
    FILE *output_file = NULL;
    char input_filename[MAX_FILE_NAME];
    char output_filename[MAX_FILE_NAME];
    char line[MAX_LINE];
    char macro_data[MAX_LINE * 5];
    char macro_name[MAX_LINE];
    char first_word[MAX_LINE];
    char *called_macro_data = NULL;
    int is_macro_flag = 0;
    int line_len;
    int data_len;
    int line_num = 0;
    
    
    /* add file extensions and create clean file without extra spaces */
    create_clean_file(filename);
    strcpy(input_filename, filename);
    strcat(input_filename, "_clean_spaces.as");
    strcpy(output_filename, filename);
    strcat(output_filename, ".am");

    /* open files to read and write */
    input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        log_error(ERROR_2);
        free_list(*macros_list);
        return 1;
    }
    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        fclose(input_file);
        log_error(ERROR_2);
        free_list(*macros_list);
        return 1;
    }

    macro_data[0]= '\0';
    while (fgets(line, MAX_LINE, input_file) != NULL) {
        line_num++;

        /* find macros decleration */
        if (is_macro_flag == 1){
            strcat(macro_data, line);
        }

        if (strncmp(line, "mcro ", 5) == 0){
            /* start of macro; intilizating macro data and saving macro name */
            is_macro_flag = 1;
            macro_data[0]= '\0';
            sscanf(line, "mcro %s", macro_name);
            if (check_valid_macro_declared(line)){
                continue;
            }
            else {
                /* macro wasnt declared right - closing files.*/
                log_error_infile(ERROR_6, filename, line_num);
                force_close(input_filename, input_file, output_file);
                return 1;
            }
        }
        
        if (strncmp(line, "mcroend", 7) == 0){
            /* end of macro; saving its data to the linked list */
            data_len = strlen(macro_data);
            line_len = strlen(line);
            macro_data[data_len - line_len] = '\0';
            
            if (check_valid_macro_declare_end(line)){
                save_macro(macro_name, macro_data, macros_list);
            }
            else{
                /* macro wasnt declared right - closing files.*/
                log_error_infile(ERROR_7, filename, line_num);
                force_close(input_filename, input_file, output_file);
                return 1;
            }
            is_macro_flag = 0;
            continue;
        }
               
        /* save to new file while replacing macros calls and ignoring macros declarations */
        if (is_macro_flag == 0){
            /* find macro calls */
            if (sscanf(line, "%s", first_word) == 1) {
                called_macro_data = search_node(*macros_list, first_word);
                /* check if first word is known macro */
                if (called_macro_data != NULL) {
                    fprintf(output_file, "%s", called_macro_data);
                }
                else {
                    fprintf(output_file, "%s", line);
                }
            }
            else {
                /* add empty rows */
                fprintf(output_file, "%s", line);
            }
        }
    }

    /* close file */
    force_close(input_filename, input_file, output_file);
    return 0;
}

