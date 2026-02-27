#ifndef UTILS_H 
#define UTILS_H

#include <stdlib.h>
#include "data_struct.h"
#define MAX_LINE 500
#define MAX_ALLOWED_LINE 81
#define MAX_FILE_NAME 100
#define MAX_LABEL_NAME 31
#define OP_CODES_COUNT 16
#define REGS_COUNT 8
#define INSTR_COUNT 6
#define MAX_MEMORY 4096
#define MIN_NUM -2048
#define MAX_NUM 2047

extern char *INSTRUCTIONS[];
extern char *REGS[];
extern char *OP_CODES[];

typedef struct command_options {
    char *command_name;
    int opcode;
    int funct;
    int operands_count;
    int source_mode[4]; /* 0-Immediate, 1-Direct, 2-Relative, 3-Register */
    int dest_mode[4];
} command_options;

extern const command_options commands_table[];

int get_command_index(char *command_name);
    /* returns the index of the command in the table */

int get_addressing_mode(char *operand);
    /* returns the addressing mode for the operand */

int is_valid_number(char *num_str);
    /* return 1 if number is valid - all chars are digits (expcet -+) and in range */

int is_valid_label(char *labelname);
    /* check is the label is valid; no longer than 31 chars, not saved word */

int arg_check(char *arg, int address_mode);
    /* returns 1 if arg is valid, else 0 */

unsigned short process_operand_first_pass(char *operand, int address_mode);
    /* return 12 bit word for operand*/

void remove_spaces_file(FILE *inputfile, FILE *outputfile);
    /* gets input and iutput files.
    run through the input file and copy text to output file without unnecessary spaces.
    removes spaces from beginning or end of line, remove spaces around commas, remove duplicate spaces, turn tabs to space. */

void create_clean_file(char *filename);
    /* gets a file name,
    open files and calls remove_spaces
    remove unnecessary spaces and tabs; duplicate, around comma, beginning or end of line. */

void force_close(char *filename, FILE *inputfile, FILE *outputfile);
    /* free memory, close files and delete tmp files */

#endif