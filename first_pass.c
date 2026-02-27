#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tables.h"
#include "errors.h"
#include "utils.h"
#include "data_struct.h"


int process_string(char *line_without_label, char *filename, int line_num, unsigned short data_image[], int *DC){
    /* read the data, covert it to ascii, add to data image, increase DC*/
    char *start_quote = NULL;
    char *end_quote = NULL;
    char *tmp_ptr = NULL;

    /* check if any args were given */
    tmp_ptr = strchr(line_without_label, ' ');
    if (tmp_ptr == NULL || *tmp_ptr == '\0') {
        log_error_infile(ERROR_14, filename, line_num);
        return 1;
    }
    /* find qoutes and check if balanced */
    start_quote = strchr(tmp_ptr, '"');
    end_quote = strrchr(tmp_ptr, '"');
    if (start_quote == NULL || end_quote == NULL || start_quote == end_quote) {
        log_error_infile(ERROR_15, filename, line_num);
        return 1;
    }
    /* check if there are more chars after second quote */
    tmp_ptr = end_quote + 1;
    while (*tmp_ptr != '\0' && *tmp_ptr != '\n') {
        if (isgraph(*tmp_ptr)) {
            log_error_infile(ERROR_16, filename, line_num);
            return 1; 
        }
        tmp_ptr++;
    }
    /* save string data and increase DC */
    start_quote++;
    for (; start_quote < end_quote; start_quote++) {
        data_image[*DC] = (unsigned short)(*start_quote);
        (*DC)++;
    }
    data_image[*DC] = 0;
    (*DC)++;
    return 0;
}

int process_data(char *line_without_label, char *filename, int line_num, unsigned short data_image[], int *DC){
    /* read the numbers, convert into 12 bits, add to data image, incraese DC*/
    char *space_ptr = NULL;
    char *number_str = NULL;
    char *scan_ptr = NULL;
    int number;
    int expecting_comma_flag = 0;
    
    /* check if any args were given */
    space_ptr = strchr(line_without_label, ' ');
    if (space_ptr == NULL || *(space_ptr + 1) == '\0') {
        log_error_infile(ERROR_10, filename, line_num); /* No args */
        return 1;
    }
    space_ptr++;
    scan_ptr = space_ptr;

    /* syntax checks - commas without numbers, consecutive commas or numbers without commas */
    while (*scan_ptr != '\0' && *scan_ptr != '\n') {
        if (isspace(*scan_ptr)){
            scan_ptr++;
            continue;
        }
        if (*scan_ptr == ',') {
            /* search for commas without numbers */
            if (!expecting_comma_flag){
                log_error_infile(ERROR_11, filename, line_num); /* consecutive commas */
                return 1;
            }
            expecting_comma_flag = 0;
            scan_ptr++;
        } 
        else if (isdigit(*scan_ptr) || *scan_ptr == '-' || *scan_ptr == '+') {
            /* search for numbers without commas */
            if (expecting_comma_flag) {
                log_error_infile(ERROR_11, filename, line_num); /* Missing comma */
                return 1;
            }
            expecting_comma_flag = 1;
            scan_ptr++;
            while (*scan_ptr != '\0' && *scan_ptr != '\n' && isdigit(*scan_ptr)) {
                scan_ptr++;
            }
        }
        else {
            log_error_infile(ERROR_12, filename, line_num);
            return 1;
        }
    }
    if (!expecting_comma_flag){
        log_error_infile(ERROR_11, filename, line_num); /* missing number after last comma */
        return 1;
    }

    /* split row into numbers and add them into data image */
    number_str = strtok(space_ptr, ",");
    while(number_str != NULL){
        if (!is_valid_number(number_str)){
            log_error_infile(ERROR_12, filename, line_num); /* number not on range or illegal number */
            return 1;
        }
        else{
            number = atoi(number_str);
            data_image[*DC] = (unsigned short)(number & 0x0FFF);
            (*DC)++;
        }
        number_str = strtok(NULL, ",");
    }
    return 0;
}

int process_extern(char *line_without_label, char *filename, int line_num, SymbolNode **symbols_table){
    /* check if the extern label is valid, and add it to symbol table */
    char extern_label[MAX_LINE];
    char extra_text[MAX_LINE];
    SymbolNode *existing_node = NULL;
    int parts_count;
    
    parts_count = sscanf(line_without_label, "%*s %s %s", extern_label, extra_text);
    if (parts_count >= 2){
        log_error_infile(ERROR_13, filename, line_num); /* extra text in line */
        return 1;
    }
    if (parts_count < 1){
        log_error_infile(ERROR_17, filename, line_num); /* no args were given */
        return 1;
    }

    if (is_valid_label(extern_label)) {
        existing_node = search_symbol_table(*symbols_table, extern_label);
        /* check if the external label is already defined as another label or external label */
        if (existing_node != NULL) {
            if (strcmp(existing_node->type, "external") == 0) {
                return 0;
            } else {
                log_error_infile(ERROR_18, filename, line_num); /* name in use */
                return 1;
            }
        }
        else {
            insert_to_symbol_table(symbols_table, extern_label, 0, "external");
            return 0;
        }
    } 
    else {
        log_error_infile(ERROR_18, filename, line_num); /* Invalid extern label */
        return 1;
    }
}

int process_command(char *line_without_label, char *filename, int line_num, unsigned short code_image[], int *IC){
    /* gets a command line and translte it into machine language,
    during the translate it check if good amount of operands passed, 
    are them written correctly, and if the addresing mode is legal */
    char *command = NULL;
    char *operands = NULL;
    char *arg1  = NULL;
    char *arg2 = NULL;
    char *comma = NULL;
    int command_index;
    int src_mode = 0;
    int dst_mode = 0;
    int L = 0;
    unsigned short word = 0;

    /* get command and operands from line */
    command = strtok(line_without_label, " \t\n");
    operands = strtok(NULL, "\n");
    command_index = get_command_index(command);
    if (command_index == -1){
        log_error_infile(ERROR_19, filename, line_num);
        return -1;
    }
    /* parse every case different, by amount of operands */
    if (commands_table[command_index].operands_count == 0){
        /* check if good amount of operands */
        if (operands != NULL && *operands != '\0') {
            log_error_infile(ERROR_21, filename, line_num); /* too much operands */
            return 1;
        }
        /* process command to bits */
        word = (commands_table[command_index].opcode << 8) | 
               (commands_table[command_index].funct << 4);
        code_image[*IC] = word;
        L = 1;
    }
    else if (commands_table[command_index].operands_count == 1){
        /* check if good anount of operands */
        if (operands == NULL || *operands == '\0') {
            log_error_infile(ERROR_20, filename, line_num); /* not enough operands */
            return 1;
        }
        arg1 = strtok(operands, " ,");
        if (strtok(NULL, " ,\t\n") != NULL) {
            log_error_infile(ERROR_21, filename, line_num); /* too much operands */
            return 1;
        }
        dst_mode = get_addressing_mode(arg1);
        if (dst_mode == -1){
            log_error_infile(ERROR_22, filename, line_num); /* Invalid addressing mode */
            return 1;
        }
        if (commands_table[command_index].dest_mode[dst_mode] == 0) {
            log_error_infile(ERROR_23, filename, line_num); /* Illegal addressing mode*/
            return 1;
        }
        if (!arg_check(arg1, dst_mode)){
            log_error_infile(ERROR_24, filename, line_num); /* Illegal operand */
        }
        /* process command and operands */
        word = (commands_table[command_index].opcode << 8) | 
               (commands_table[command_index].funct << 4) |
               (0 << 2) |
               (dst_mode << 0);
        code_image[*IC] = word;
        code_image[*IC + 1] = process_operand_first_pass(arg1, dst_mode);
        L = 2; 
    }
    else if (commands_table[command_index].operands_count == 2){
        /* check if good amount of operands */
        comma = strchr(operands, ',');
        if (operands == NULL || *operands == '\0' || comma == NULL) {
            log_error_infile(ERROR_20, filename, line_num); /* not enough operands */
            return 1;
        }
        *comma = '\0';
        arg1 = strtok(operands, " \t\n");
        arg2 = strtok(comma + 1, " \t\n");
        if (arg1 == NULL || arg2 == NULL) {
            log_error_infile(ERROR_20, filename, line_num); /* not enough operands */
            return 1;
        }
        if (strtok(NULL, " \t\n") != NULL) {
            log_error_infile(ERROR_21, filename, line_num); /* too much operands */
            return 1;
        }
        src_mode = get_addressing_mode(arg1);
        dst_mode = get_addressing_mode(arg2);
        if (src_mode == -1 || dst_mode == -1){
            log_error_infile(ERROR_22, filename, line_num); /* Invalid addressing mode */
            return 1;
        }
        if (commands_table[command_index].source_mode[src_mode] == 0 || 
            commands_table[command_index].dest_mode[dst_mode] == 0){
                log_error_infile(ERROR_23, filename, line_num); /* Illegal addressing method */
        }
        if (!arg_check(arg1, src_mode) || !arg_check(arg2, dst_mode)){
            log_error_infile(ERROR_24, filename, line_num); /* Illegal operand */
        }
        /* process command and operands */
        word = (commands_table[command_index].opcode << 8) | 
               (commands_table[command_index].funct << 4) |
               (src_mode << 2) |
               (dst_mode << 0);
        code_image[*IC] = word;
        code_image[*IC + 1] = process_operand_first_pass(arg1, src_mode);
        code_image[*IC + 2] = process_operand_first_pass(arg2, dst_mode);
        L = 3;
    }
    (*IC) += L;
    return 0;
}

int handle_directive(char *line_without_label, char *first_word, char *filename, int line_num,
     unsigned short data_image[], int *DC, SymbolNode **symbols_table, int is_label_flag){
    /* handles line that are directive by their type */
    if (strstr(line_without_label, ".string")){
        /* save label with type data*/
        if (is_label_flag){
            insert_to_symbol_table(symbols_table, first_word, *DC, "data");
        }
        /* process string - saving to data image and updating DC */
        return process_string(line_without_label, filename, line_num, data_image, DC);
    }
    else if (strstr(line_without_label, ".data")){
        /* save label with type data */
        if (is_label_flag){
            insert_to_symbol_table(symbols_table, first_word, *DC, "data");
        }
        /* process data - saving to data image and updating DC*/
        return process_data(line_without_label, filename, line_num, data_image, DC);
    }
    else if (strstr(line_without_label, ".extern")){
        /* check if extern is valid and add it to the symbol table */
        return process_extern(line_without_label, filename, line_num, symbols_table);
    }
    else if(strstr(line_without_label, ".entry")){
        /* dont do anthing */
        return 0;
    }
    /* invalid type of directive */
    log_error_infile(ERROR_9, filename, line_num);
    return 1;
}

int first_pass(char * filename, Node *macros_list){
    /* execute first pass */
    SymbolNode *symbols_table = NULL;
    unsigned short code_image[MAX_MEMORY];
    unsigned short data_image[MAX_MEMORY];
    int IC = 100, DC = 0, ICF = 100, DCF = 0;
    FILE *input_file = NULL;
    char input_filename[MAX_FILE_NAME];
    char line[MAX_LINE];
    char first_word[MAX_LINE];
    int len;
    int line_num = 0;
    char line_without_label[MAX_LINE];
    char *space_ptr = NULL;    
    int is_label_flag = 0;
    int error_flag = 0;
    int i; /* <---- delete, just for debug */

    /* add file extensions and create clean file without extra spaces */
    strcpy(input_filename, filename);
    strcat(input_filename, ".am");

    /* open files to read and write */
    input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        log_error(ERROR_2);
        return 1;
    }
    
    while (fgets(line, MAX_LINE, input_file) != NULL) {
        /* main loop - go through every line in the file and process it */
        is_label_flag = 0;
        line_num ++;
        if (strlen(line) > MAX_ALLOWED_LINE){
            /* check if line too long */
            log_error_infile(ERROR_5, filename, line_num);
            error_flag = 1;
            continue;
        }
        if (strcmp(line, "\n") == 0 || strncmp(line, ";", 1) == 0){
            /* check if comment or empty line*/
            continue;
        }

        else if (sscanf(line, "%s", first_word) == 1){
            /* check if theres a label and create a copy of the line without it */
            len = strlen(first_word);
            if (first_word[len - 1] == ':') {
                first_word[len - 1] = '\0';
                if (is_valid_label(first_word) && search_symbol_table(symbols_table, first_word) == NULL
                    && search_node(macros_list, first_word) == NULL){
                    /* check if label name is valid */
                    is_label_flag = 1;
                    space_ptr = strchr(line, ' ');
                    if (space_ptr != NULL){
                        strcpy(line_without_label, space_ptr + 1);
                    }
                    else {
                        line_without_label[0] = '\0';
                    }
                }
                else {
                    log_error_infile(ERROR_8, filename, line_num); /* label naem invalid or lbael in table */
                    error_flag = 1;
                    continue;
                }
            }
            else {
                strcpy(line_without_label, line);
            }
        }
        if (line_without_label[0] == '\0' || line_without_label[0] == '\n') {
            /* empty line after label declaration */
            continue;
        }
        else if (line_without_label[0] == '.'){
            /* the line is a directive */
            if (handle_directive(line_without_label, first_word, filename,
                line_num, data_image, &DC, &symbols_table, is_label_flag)){
                    error_flag = 1;
            }
        }
        else {
            /* the line is command */
            if (is_label_flag == 1) {
                insert_to_symbol_table(&symbols_table, first_word, IC, "code");
            }
            if (process_command(line_without_label, filename, line_num, code_image, &IC)){
                /* error in instruction line */
                error_flag = 1;
            }
        }
    }

    if (error_flag){
        printf("error in first pass, not going into second pass\n");
        free_symbol_table(symbols_table);
        fclose(input_file);
        return 1;
    }

    ICF = IC;
    DCF = DC;
    update_data_address(symbols_table, IC);

    /* ---------------------------- */
    /* ----- change from here ----- */
    /* ---------------------------- */
    printf("\n--- Final Symbol Table ---\n");
    print_symbol_table(symbols_table);

    printf("\n--- Code Image (IC: 100 to %d) ---\n", ICF);
    for (i = 100; i < ICF; i++) {
        /* Print address and value in Hex (3 digits for 12 bits) */
        printf("%04d: %03X\n", i, code_image[i]);
    }

    printf("\n--- Data Image ---\n");
    for (i = 0; i < DCF; i++) {
        printf("%04d: %03X\n", ICF + i, data_image[i]);
    }

    free_symbol_table(symbols_table);
    fclose(input_file);
    return 0;
}