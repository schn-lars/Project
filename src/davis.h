#ifndef DAVIS_H
#define DAVIS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "commands/quit.h"
#include "commands/ls.h"

#define MAX_INPUT_BUFFER    128 // Including zero byte at the end
#define MAX_COMMAND_LENGTH  10
#define MAX_INPUT_COUNT     8
extern int shell_running;

void davis();
void get_input();
void parse_input_into_commands();
void exec_command();
void notify(char *message);
void warn(char *warning);
int cast_flag_into_int(char *flag);

#endif