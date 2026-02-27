#ifndef ERRORS_H 
#define ERRORS_H


typedef struct Error {
    int error_id;
    char *error_msg;
} Error;

typedef enum ERROR_CODES {
    /* The following errors are macro related or general */
    ERROR_0 = 0,
    ERROR_1,
    ERROR_2,
    ERROR_3,
    ERROR_4,
    ERROR_5,
    ERROR_6,
    ERROR_7,
    ERROR_8,
    ERROR_9,
    ERROR_10,
    ERROR_11,
    ERROR_12,
    ERROR_13,
    ERROR_14,
    ERROR_15,
    ERROR_16,
    ERROR_17,
    ERROR_18,
    ERROR_19,
    ERROR_20,
    ERROR_21,
    ERROR_22,
    ERROR_23,
    ERROR_24
} ERROR_CODES;

void log_error_infile(int error_code, char *filename, int line_count);
    /* prints to stdout error code, message, file name and line */



void log_error(int error_code);
    /* prints to stdout error code and message */


#endif