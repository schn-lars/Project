#ifndef DAVIS_H
#define DAVIS_H


#define MAX_INPUT_BUFFER    1024 // Including zero byte at the end
#define MAX_COMMAND_LENGTH  10
#define MAX_ARG_LENGTH      64
#define MAX_INPUT_COUNT     30
#define MAX_CMD_COUNT       2
#define LOGGING             0 // 1 = enabling logging statements
#define SCRATCH_FILE        "./shared-file"
#define FAILURE             0
#define SUCCESS             1

struct Input {
    char *cmd_one[MAX_INPUT_COUNT];
    char *cmd_two[MAX_INPUT_COUNT];
    int no_commands;
};

#include "commands/quit.h"
#include "commands/ls.h"
#include "commands/help.h"
#include "commands/clear.h"
#include "commands/plot.h"
#include "commands/hist.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>


extern int shell_running;



void davis();
void get_input();
void parse_input_into_commands();
void exec_command();
void notify(char *message);
void warn(char *warning);
void LOGGER(char *desc, char *log_statement);
void chain_up_flags(char **parsed_sorted_input);
void sort_flags_in_arguments(char **parsed_input);
void print_arguments();
void put_flags_first(char **chained_up_flags);
void clear_input_struct();

#endif