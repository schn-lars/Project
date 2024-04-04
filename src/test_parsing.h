#ifndef TEST_PARSING_H
#define TEST_PARSING_H

#include "string.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"

#define MAX_COMMAND_LENGTH  10
#define MAX_INPUT_COUNT     10
#define MAX_CMD_COUNT       2

void clear_args();
int setup_tests();
int execute_test(char *test_arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT], char *expected[MAX_CMD_COUNT][MAX_INPUT_COUNT]);
int main();
void chain_up_flags();
void sort_flags_in_arguments();
void put_flags_first();
int compareArrays(char *exp[MAX_CMD_COUNT][MAX_INPUT_COUNT], char *act[MAX_CMD_COUNT][MAX_INPUT_COUNT]);
void print_arguments();
void copy_arrays(char *test_arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT], char *expected[MAX_CMD_COUNT][MAX_INPUT_COUNT]);

#endif