#include "davis.h"

/*
 * This is the main access point for our custom shell.
 */

char input[MAX_INPUT_BUFFER];
char *command[MAX_CMD_COUNT][1];
int no_command = 0;
char *arguments[MAX_CMD_COUNT][4];
int input_length;
int shell_running = 1;
char *log_msg;

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
        LOGGER("parsing");
        parse_input_into_commands();
        exec_command();
        cleanup();
    }
}

/*
 * This method takes user input.
 */
void get_input() {
    printf("[DAVIS]\t");
    if (fgets(input, MAX_INPUT_BUFFER, stdin) == NULL) {
        warn("Error reading input.");
        return;
    }
    LOGGER("cpsn");
    input[strcspn(input, "\n")] = '\0';
    LOGGER("cpsn done");
}



/*
 * Translating the given input in syntactically correct parameters.
 */
void parse_input_into_commands() {
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        LOGGER("aussen");
        command[i][0] = NULL;
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            LOGGER("innen");
            arguments[i][j] = NULL;
        }
    }

    LOGGER("initialization done");
    int word_counter = 0;
    char *token = strtok(input, " ");
    while (token != NULL) {
        if (strcmp(token, "|") == 0) {
            if (word_counter == 0 && no_command == 0) { // Pipe cannot be first token
                warn("Wrong usage of pipe. No command specified before pipe.");
                return;
            }
            if (no_command >= MAX_CMD_COUNT) {
                warn("Too many commands. Ignoring excess commands.");
                return;
            }
            no_command++; // Increase the number of commands
            word_counter = 0; // Reset word counter
        } else {
            if (command[no_command][0] == NULL) { // First word after pipe or start
                command[no_command][0] = strdup(token);
            } else {
                arguments[no_command][word_counter - 1] = strdup(token); // Dynamically allocate memory for each argument
            }
            word_counter++;
        }
        token = strtok(NULL, " ");
    }
    if (command[1][0] != NULL) {
        no_command++;
    }

    // Output for debugging purposes
    for (int i = 0; i < no_command; i++) {
        printf("Command %d: %s\n", i + 1, command[i][0]);
        printf("Arguments for Command %d:\n", i + 1);
        for (int j = 0; j < 4; j++) {
            if (arguments[i][j] != NULL) {
                printf("%s ,\n", arguments[i][j]);
            }
        }
    }
    LOGGER("End of parsing.");
}

/*
 * Executes the command, previously saved in command originating from input.
 * Commands can be added by elif-statements.
 */
void exec_command() {
    LOGGER("starting in exec_command()");
    if (no_command == 2) { // Piping
        printf("Start piping ...");
        // command 0 is being executed first.
        int fd[2];
        pid_t p1, p2;
        if (pipe(fd) < 0) {
            warn("Couldn't perform piped command.");
            return;
        }
        p1 = fork();
        if (p1 < 0) {
            warn("Problem occurred while forking.");
            return;
        }

        if (p1 == 0) {
            // Setting tunnels for communication between the two processes.
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            if (execvp(command[0][0], arguments[0]) < 0) {
                warn("First command failed.");
                exit(0);
            }
        } else {
            p2 = fork();
            if (p2 < 0) {
                warn("Problem occurred while forking.");
                return;
            }

            // Child 2 executing..
            // It only needs to read at the read end
            if (p2 == 0) {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                if (execvp(command[1][0], arguments[1]) < 0) {
                    warn("Second command failed.");
                    exit(0);
                }
            } else {
                close(fd[0]); // Close both ends of pipe in parent
                close(fd[1]);
                wait(NULL);
                wait(NULL);
            }
        }
    } else {
        // Regular command
        LOGGER("Executing regular command.");
        if (strcmp(command[0][0], "quit") == 0) {
            quit();
        } else if (strcmp(command[0][0], "ls") == 0) {
            LOGGER("Calling ls");
            ls(cast_flag_into_int(arguments[0][0]), "."); // TODO argumente korrekt empfangen
        } else if (strcmp(command[0][0], "help") == 0) {
            LOGGER("Calling help");
            help();
        } else {
            warn("Unknown command.");
        }
    }
    no_command = 0;
}

/*
 * Helpful for executing commands. Easier to compare integers rather than strings.
 */
// @Deprecated
int cast_flag_into_int(char *flag) {
    LOGGER(flag);
    if (flag == NULL) {
        return 0;
    } else if (flag[0] != '-') {
        warn("Wrongful usage of flag. Try '-[FLAG]'.");
        return -1;
    } else if (strcmp(flag, "-l")) {
        return 1;
    } else if (strcmp(flag, "-a")) {
        return 2;
    } else if (strcmp(flag, "-al")) {
        return 3;
    }
    return 0;
}

void cleanup() {
    LOGGER("cleanup()");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
            if (arguments[i][j] != NULL) {
                free(arguments[i][j]);
                arguments[i][j] = NULL;
            }
        }
    }
}

/*
 * For testing purposes
 */
void LOGGER(char *log_statement) {
    if (LOGGING) {
        printf("[LOGGER] %s\n", log_statement);
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

