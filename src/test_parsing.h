#ifndef TEST_PARSING_H
#define TEST_PARSING_H

#include "string.h"
#include "davis.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"

#define MAX_COMMAND_LENGTH  10
#define MAX_INPUT_COUNT     10
#define MAX_CMD_COUNT       2

void malloc_struct();
void clear_args();
int setup_tests();
int execute_test();
int main();
void chain_up_flags();
void sort_flags_in_arguments();
void put_flags_first();
int compareArrays();
void print_arguments();
void free_struct();

#endif