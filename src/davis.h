#ifndef DAVIS_H
#define DAVIS_H

#include "commands/quit.h"
#include "commands/ls.h"
#include "commands/help.h"
#include "commands/clear.h"
#include "commands/plot.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_INPUT_BUFFER    256 // Including zero byte at the end
#define MAX_COMMAND_LENGTH  10
#define MAX_INPUT_COUNT     10
#define MAX_CMD_COUNT       2
#define LOGGING             1 // 1 = enabling logging statements
extern int shell_running;

void davis();
void get_input();
void parse_input_into_commands();
void exec_command();
void notify(char *message);
void warn(char *warning);
int cast_flag_into_int(char *flag);
void cleanup();
void LOGGER(char *desc, char *log_statement);
void chain_up_flags();
void sort_flags_in_arguments();
void print_arguments();
void put_flags_first();

#endif