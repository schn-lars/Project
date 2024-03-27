#include "davis.h"

/*
 * This is the main access point for our custom shell.
 */

char input[MAX_INPUT_BUFFER];
char *command[MAX_CMD_COUNT][1];
int no_command = 0;
char *arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT];
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
        get_input(); // ls q w -l e -o -p
        LOGGER("parsing", "");
        parse_input_into_commands(); // korrekt geparst
        sort_flags_in_arguments();
        //chain_up_flags();
        LOGGER("PRINT ARGS", "");
        print_arguments();
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
    input[strcspn(input, "\n")] = '\0';
}



/*
 * Translating the given input in syntactically correct parameters.
 */
void parse_input_into_commands() {
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        command[i][0] = NULL;
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            arguments[i][j] = NULL;
        }
    }
    int word_counter = 0;
    char *token = strtok(input, " ");
    while (token != NULL) {
        LOGGER("Token:", token);
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
                LOGGER("1st cmd NULL", "");
                command[no_command][0] = strdup(token);
                LOGGER(command[no_command][0], "");
            } else {
                arguments[no_command][word_counter] = strdup(token); // Dynamically allocate memory for each argument
                word_counter++;
            }
        }
        token = strtok(NULL, " ");
    }
    no_command++;
    LOGGER("End of parsing.", "");
}

/*
 * Executes the command, previously saved in command originating from input.
 * Commands can be added by elif-statements.
 */
void exec_command() {
    LOGGER("starting in exec_command()",arguments[0][0]);
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
        LOGGER("Executing regular command: ", command[0][0]);
        if (strcmp(command[0][0], "quit") == 0) {
            quit();
        } else if (strcmp(command[0][0], "ls") == 0) {
            LOGGER("Calling ls", "");
            ls(cast_flag_into_int(arguments[0][0]), "."); // TODO argumente korrekt empfangen
        } else if (strcmp(command[0][0], "help") == 0) {
            LOGGER("Calling help: ", "");
            help();
        } else if (strcmp(command[0][0], "clear") == 0) {
            LOGGER("Calling clear: ", arguments[0][0]);
            clear();
        } else {
            warn("Unknown command.");
        }
    }
    no_command = 0;
}

/*
 * This implements the insertion sort algorithm and pushes flags to the left and rest to the right.
 */
void sort_flags_in_arguments()
{
    LOGGER("Sorting flags ...", "");
    for (int i = 0; i < MAX_CMD_COUNT; i++) { // Iterates over commands
        for (int j = 0; j < MAX_INPUT_COUNT - 1; j++) { // Iterates over elements in the array
            for (int k = 0; k < MAX_INPUT_COUNT - 1 - j; k++) {
                if (arguments[i][k] == NULL || arguments[i][k+1] == NULL) {
                    continue; // ignore NULL-elements
                }

                // Checking if elements are flags
                int isFlagK = (arguments[i][k][0] == '-');
                int isFlagK1 = (arguments[i][k+1][0] == '-');

                // is current element is flag or next?
                // or both are flags and current came first
                if ((isFlagK && !isFlagK1) || (isFlagK && isFlagK1 && strcmp(arguments[i][k], arguments[i][k+1]) > 0)) {
                    // swapping
                    char *temp = arguments[i][k];
                    arguments[i][k] = arguments[i][k + 1];
                    arguments[i][k + 1] = temp;
                }
            }
        }

        // Sort not-flags
        for (int j = 0; j < MAX_INPUT_COUNT - 1; j++) { // Iterates over elements in the array
            for (int k = 0; k < MAX_INPUT_COUNT - 1 - j; k++) {
                if (arguments[i][k] == NULL || arguments[i][k+1] == NULL) {
                    continue;
                }

                int isFlagK = (arguments[i][k][0] == '-');
                int isFlagK1 = (arguments[i][k+1][0] == '-');

                // both are not flags
                if ((!isFlagK && !isFlagK1) || (!isFlagK && isFlagK1)) {
                    // swap
                    char *temp = arguments[i][k];
                    arguments[i][k] = arguments[i][k + 1];
                    arguments[i][k + 1] = temp;
                }
            }
        }
    }
    LOGGER("After sort:", "");
    print_arguments();
}

/*
 *  Puts every flag in input into one single bigger flag (f.e. -l -a -> -la)
 */
void chain_up_flags() {
    LOGGER("Chaining up.", "");
    char combined_flags[MAX_INPUT_COUNT];
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        combined_flags[i] = '\0';
    }
    int found_flag = 0;

    for (int i = 0; i < no_command; i++) {
        for (int k = 0; k < MAX_INPUT_COUNT; k++) {
            combined_flags[k] = '\0';
        }
        for (int j = 0; j < MAX_INPUT_COUNT; j++) { // Iterate over arguments looking for flags.
            if (arguments[i][j] != NULL && arguments[i][j][0] == '-') {
                strcat(combined_flags, arguments[i][j] + 1); // Extracting string
                arguments[i][j] = NULL;
                found_flag++;
            }
        }

        if (found_flag != 0) {
            arguments[i][MAX_INPUT_COUNT - 1] = strdup(combined_flags);
            found_flag = 0;
            LOGGER("Combined flags: ",arguments[i][0]);
            //put_flags_first();
        }
    }
    LOGGER("After chain.", "");
    print_arguments();
}

void put_flags_first()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = MAX_INPUT_COUNT - 2; j >= 0; j--) {
            if (arguments[i][j] != NULL) {
                arguments[i][j + 1] = arguments[i][j];
            }
        }
    }
}

/*
 * Helpful for executing commands. Easier to compare integers rather than strings.
 */
// @Deprecated
int cast_flag_into_int(char *flag) {
    LOGGER("Casting flags:",flag);
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

/*
 * Frees argument array
 */
void cleanup() {
    LOGGER("cleanup()", "");
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
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
void LOGGER(char *desc, char *log_statement) {
    if (LOGGING) {
        printf("[LOGGER] %s ", desc);
        printf(" %s\n", log_statement);
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
    printf("ERROR - %s\n", warning);
}

void print_arguments()
{
    for (int i = 0; i < no_command; i++) {
        printf("Command %d: %s\n", i + 1, command[i][0]);
        printf("Arguments for Command %d:\n", i + 1);
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            //if (arguments[i][j] != NULL) {
                printf("%s,\n", arguments[i][j]);
            //}
        }
    }
}

