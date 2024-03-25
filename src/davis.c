#include "davis.h"

/*
 * This is the main access point for our custom shell.
 */

char input[MAX_INPUT_BUFFER];
char command[MAX_COMMAND_LENGTH];
char *arguments[8];
int input_length;
int shell_running = 1;

int main()
{
    printf("░▒▓███████▓▒░   ░▒▓██████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓███████▓▒░ \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░ ░▒▓█▓▒░        \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓█▓▒▒▓█▓▒░  ░▒▓█▓▒░ ░▒▓█▓▒░        \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓████████▓▒░  ░▒▓█▓▒▒▓█▓▒░  ░▒▓█▓▒░  ░▒▓██████▓▒░  \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▓█▓▒░   ░▒▓█▓▒░        ░▒▓█▓▒░ \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▓█▓▒░   ░▒▓█▓▒░        ░▒▓█▓▒░ \n");
    printf("░▒▓███████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░    ░▒▓██▓▒░    ░▒▓█▓▒░ ░▒▓███████▓▒░  \n");
    printf("\n");
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
 * Commands can be added by elif-statements.
 */
void exec_command() {
    if (strcmp(command, "quit") == 0) {
        quit();
    } else if (strcmp(command, "ls") == 0) {
        ls(command[0],".");
    } else {
        notify("Unknown command.");
    }

    // Set arguments and command to null after usage.
    memset(command, 0, sizeof(command));
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        arguments[i] = NULL;
    }
}

/*
 * Used to send messages to the client's interface.
 */
void notify(char *message)
{
    printf("[DAVIS] %s\n", message);
}

/*
 * This method gets called by any errors regarding wrong user input.
 */
void warn(char *warning)
{
    printf("ERROR - %s", warning);
}

/*
 * Helpful for executing commands. Easier to compare integers rather than strings.
 */
int cast_flag_into_int(char *flag)
{
    if (flag[0] != '-') {
        warn("Wrongful usage of flag. Try '-[FLAG]'.")
        return -1;
    }

}
