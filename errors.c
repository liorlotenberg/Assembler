#include <stdio.h>
#include <string.h>
#include "errors.h"


Error errors[] ={
    {ERROR_0, "No Error"},
    {ERROR_1, "Failed to create file"},
    {ERROR_2, "Failed to open file"},
    {ERROR_3, "Failed to allocate memory"},
    {ERROR_4, "====="},
    {ERROR_5, "Line too long"},
    {ERROR_6, "Illegal macro declaration"},
    {ERROR_7, "Extra chars in mcroend line"},
    {ERROR_8, "Invalid label declaires - Invalid name or already declaired"},
    {ERROR_9, "Invalid directive"},
    {ERROR_10, "No args were given after .data"},
    {ERROR_11, "Invalid order in .data - consecutive commas, numbers withoud commas or comma without number"},
    {ERROR_12, "Given number not in range or has char in it"},
    {ERROR_13, "Extra text given in .extern"},
    {ERROR_14, "No args were given after .string"},
    {ERROR_15, "Unbalanced quates in .string"},
    {ERROR_16, "Text after qoute in .string"},
    {ERROR_17, "No args were given after .extern"},
    {ERROR_18, "Invalid extern label name"},
    {ERROR_19, "Invalid command type"},
    {ERROR_20, "Missing operand"},
    {ERROR_21, "Too much operands passed"},
    {ERROR_22, "Invalid addressing method"},
    {ERROR_23, "Illegal addressing mode for this command"}, 
    {ERROR_24, "Illegal operand passed"}
};


void log_error_infile(int error_code, char *filename, int line_count){
    /* prints to stdout error code, message, file name and line */
    printf("Error ID: %d | Error MSG: %s | File name: %s  | Line: %d\n", error_code, errors[error_code].error_msg, filename, line_count);
}


void log_error(int error_code){
    /* prints to stdout error code and message */
    printf("Error ID: %d | Error MSG: %s\n", error_code, errors[error_code].error_msg);
}