#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/*
 * This is the main access point for our custom shell.
 */

#define MAX_INPUT_BUFFER    128
#define MAX_INPUT_COUNT     8

char input[MAX_INPUT_BUFFER];
pid_t pid; // Process number
char *argv[8];

/*
 * Declaring functions for our shell.
 */
void davis();
void get_input();
void exec_input();

int main()
{
    // Opening shell.
    davis();

    return 0;
}

void davis()
{
    // TODO vielleicht eine grafische Fläche zu Beginn
    printf("Hey, I'm DAVIS. How may I assist You?\n");
    while (1) {
        get_input();
        exec_input();
        if(!strcmp("quit", input)) break;
    }
}

/*
 * This method takes user input.
 */
void get_input()
{
    printf("[DAVIS]\t");
    fgets(input, MAX_INPUT_BUFFER, stdin);
    int input_length = strlen(input);
    if ((input_length > 0) && (input[input_length - 1] == '\n'))
        input[input_length - 1] = '\0'; // Terminate string with zero-byte
}