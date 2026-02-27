#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "errors.h"
#include "data_struct.h"

char *INSTRUCTIONS[] = {"data", "string", "entry", "extern", "mcro", "mcroend"};
char *REGS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char *OP_CODES[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", 
    "jsr", "red", "prn", "rts", "stop"};

const command_options commands_table[] = {
    /* command_name, opcode, funct, operands_count, source_mode, dest_mode */
    /* 0-Immediate, 1-Direct, 2-Relative, 3-Register */
    {"mov",  0,  0, 2, {1, 1, 0, 1}, {0, 1, 0, 1}}, 
    {"cmp",  1,  0, 2, {1, 1, 0, 1}, {1, 1, 0, 1}},
    {"add",  2,  10, 2, {1, 1, 0, 1}, {0, 1, 0, 1}},
    {"sub",  2,  11, 2, {1, 1, 0, 1}, {0, 1, 0, 1}},    
    {"lea",  4,  0, 2, {0, 1, 0, 0}, {0, 1, 0, 1}},
    {"clr",  5,  10, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"not",  5,  11, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"inc",  5,  12, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"dec",  5,  13, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"jmp",  9,  10, 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
    {"bne",  9, 11, 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
    {"jsr",  9, 12, 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
    {"red",  12, 0, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"prn",  13, 0, 1, {0, 0, 0, 0}, {1, 1, 0, 1}},
    {"rts",  14, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {"stop", 15, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}}
};

int get_command_index(char *command_name){
    /* returns the index of the command in the table */
    int i = 0;
    for (; i < 16; i++){
        if (strcmp(command_name, commands_table[i].command_name) == 0){
            return i;
        }
    }
    return -1; /* command not in table */
}

int get_addressing_mode(char *operand) {
    /* returns the addressing mode for the operand */
    if (operand[0] == '#') {
        /* Immediate */
        return 0;
    }
    if (operand[0] == '%') {
        /* Relative */
        return 2;
    }
    if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' && operand[1] <= '7') {
        /* Register */
        return 3;
    }
    if (is_valid_label(operand)) {
        /* Direct */
        return 1;
    }
    return -1;
}

int is_valid_number(char *num_str){
    /* return 1 if number is valid - all chars are digits (expcet -+) and in range */
    char *ptr = num_str;
    int num;
    int len;    
    if (num_str == NULL || *num_str == '\0') {
        return 0;
    }
    if (*ptr == ' '){
        ptr++;
    }
    if (*ptr == '+' || *ptr == '-') {
        ptr++;
        if (*ptr == '\0') return 0;
    }
    len = strlen(ptr);
    if (ptr[len - 1] == '\n' || ptr[len - 1] == ' ') {
        ptr[len - 1] = '\0';
    }
    while (*ptr != '\0') {
        if (!isdigit(*ptr)) {
            return 0;
        }
        ptr++;
    }

    num = atoi(num_str);
    if (num < MIN_NUM || num > MAX_NUM) {
        return 0;
    }

    return 1;
}

int is_valid_label(char *labelname){
    /* check is the label is valid; no longer than 31 chars, not saved word */
    int i;
    /* check length */
    if (strlen(labelname) > MAX_LABEL_NAME ){
        return 0;
    }
    /* check first char */
    if (!isalpha(labelname[0])){
        return 0;
    }
    /* check if not saved word */
    for (i = 0; i < INSTR_COUNT; i++){
        if (strcmp(labelname, INSTRUCTIONS[i]) == 0) {
            return 0;
                    }
    }
    for (i = 0; i < REGS_COUNT; i++){
        if (strcmp(labelname, REGS[i]) == 0) {
            return 0;
        }
    }   
    for (i = 0; i < OP_CODES_COUNT; i++){
        if (strcmp(labelname, OP_CODES[i]) == 0) {
            return 0;
        }
    }
    /* ----- check if not macro name */
    return 1;
}

int arg_check(char *arg, int address_mode){
    /* returns 1 if arg is valid, else 0 */
    char *opernad_without_prefix = NULL;
    if (address_mode == 0){
        opernad_without_prefix = arg + 1;
        if (is_valid_number(opernad_without_prefix) == 0){
                return 0; /* number not in range or illgeal number */
            }
        return 1;
    }
    if (address_mode == 1){
        if (is_valid_label(arg)){
            return 1;
        }
        return 0;
    }
    if (address_mode == 2){
        if (is_valid_label(arg + 1)){
            return 1;
        }
        return 0;
    }
    return 1;
}

unsigned short process_operand_first_pass(char *operand, int address_mode) {
    /* return 12 bit word for operand*/
    int value = 0;
    if (address_mode == 0) {
        value = atoi(operand + 1);        
        return (unsigned short)(value & 0xFFF);
    }
    else if (address_mode == 3) {
        value = atoi(operand + 1);
        return (unsigned short)(1 << value);
    }
    return 0;
}

void remove_spaces_file(FILE *inputfile, FILE *outputfile){
    /* gets input and iutput files.
    run through the input file and copy text to output file without unnecessary spaces.
    removes spaces from beginning or end of line, remove spaces around commas, remove duplicate spaces, turn tabs to space. */

    char current_char;
    char prev_char = '\n';
    int space_pending_flag = 0; 

    while ((current_char = fgetc(inputfile)) != EOF) {
        /* each char can be: (1) space or tab (2) comma (3) new line (4) normal */

        if (current_char == ' ' || current_char == '\t') {
            /* if tab or space, set flag */
            space_pending_flag = 1;
        } 
               
        else if (current_char == '\n') {
            /* remove spaces from end of line by reseting the flag and printing new line */
            space_pending_flag = 0;
            fputc(current_char, outputfile);
            prev_char = '\n';
        }
        
        else if (current_char == ',') {
            /* remove spaces before comma by reseting the flag */
            space_pending_flag = 0;
            fputc(',', outputfile);
            prev_char = ',';
        }
        
        else {            
            if (space_pending_flag == 1) {
                if (prev_char != ',' && prev_char != '\n') {
                    /* add space only if it not after comma or new line */
                    fputc(' ', outputfile);
                }
                space_pending_flag = 0; 
            }
            /* add the normal char to file */
            fputc(current_char, outputfile);
            prev_char = current_char;
        }
    }
}


void create_clean_file(char *filename){
    /* gets a file name,
    open files and calls remove_spaces
    remove unnecessary spaces and tabs; duplicate, around comma, beginning or end of line. */
    char input_file_name[MAX_FILE_NAME];
    char output_file_name[MAX_FILE_NAME];
    FILE *inputfile;
    FILE *outputfile;
    
    strcpy(input_file_name, filename);
    strcat(input_file_name, ".as");
    strcpy(output_file_name, filename);
    strcat(output_file_name, "_clean_spaces.as");

    inputfile = fopen(input_file_name, "r");
    if (inputfile == NULL) {
        log_error(ERROR_2);
        return;
    }
    outputfile = fopen(output_file_name, "w");
    if (outputfile == NULL) {
        fclose(inputfile);
        log_error(ERROR_1);
        return;
    }
    
    remove_spaces_file(inputfile, outputfile);

    fclose(inputfile);
    fclose(outputfile);
}


void force_close(char *filename, FILE *inputfile, FILE *outputfile){
    /* close files and delete tmp files */
    char delete_command[MAX_FILE_NAME] = "rm ";
    strcat(delete_command, filename);
    system(delete_command);
    if (inputfile != NULL) {
        fclose(inputfile);
    }
    if (outputfile != NULL) {
        fclose(outputfile);
    }
}