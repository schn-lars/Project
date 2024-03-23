#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"

/*
 * This is the main access point for our custom shell.
 */

#define MAX_INPUT_BUFFER    128 // Including zero byte at the end
#define MAX_COMMAND_LENGTH  10
#define MAX_INPUT_COUNT     8

char input[MAX_INPUT_BUFFER];
char command[MAX_COMMAND_LENGTH];
pid_t pid; // Process number
char *arguments[8];
int input_length;
int shell_running;

/*
 * Declaring functions for our shell.
 */
void davis();
void get_input();
void parse_input_into_commands();
void exec_command();


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
    shell_running = 1;
    while (shell_running) {
        get_input();
        parse_input_into_commands();
        exec_command();
    }
}

/*
 * This method takes user input.
 */
void get_input()
{
    printf("[DAVIS]\t");
    fgets(input, MAX_INPUT_BUFFER, stdin);
    input_length = strlen(input);
    if ((input_length > 0) && (input[input_length - 1] == '\n'))
        input[input_length - 1] = '\0'; // Terminate string with zero-byte
}

/*
 * Translating the given input in syntactically correct parameters.
 */
void parse_input_into_commands() {
    int word_counter = 0;
    char *token = strtok(input, " ");
    if (token != NULL) {
        // The first token is the command itself.
        strncpy(command, token, MAX_COMMAND_LENGTH - 1);
        command[MAX_COMMAND_LENGTH - 1] = '\0';

        // Cut arguments into pieces. If there are more arguments than 10, they would simply be ignored.
        while ((token = strtok(NULL, " ")) != NULL && word_counter < MAX_INPUT_COUNT) {
            arguments[word_counter] = token;
            word_counter++;
        }
    }
}

/*
 * Executes the command, previously saved in command originating from input.
 */
void exec_command() {
    printf("Command: %s\n", command);

    if (strcmp(command, "quit") == 0) {
        shell_running = 0;
    } else {
        printf("Unknown command: %s\n", command);
    }

    // Set arguments and command to null after usage.
    memset(command, 0, sizeof(command));
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        arguments[i] = NULL;
    }
}
