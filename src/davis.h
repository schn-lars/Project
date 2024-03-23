#ifndef DAVIS_H
#define DAVIS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAX_INPUT_BUFFER    128 // Including zero byte at the end
#define MAX_COMMAND_LENGTH  10
#define MAX_INPUT_COUNT     8

char input[MAX_INPUT_BUFFER];
char command[MAX_COMMAND_LENGTH];
pid_t pid; // Process number
char *arguments[8];
int input_length;
int shell_running;

void davis();
void get_input();
void parse_input_into_commands();
void exec_command();
void notify(char *message);

#endif