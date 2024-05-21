#include "davis.h"

/*
 * This is the main access point for our custom shell.
 */

char *input;
struct Input *in;
struct Purse *purse;
int second_cmd_procedure;
int shell_running = 1;

pthread_mutex_t  input_mutex;
pthread_t arrow_thread;
struct Node *curr_cmd;
int total_commands = 0;
int history_command = -1;

struct termios orig_termios;

int main()
{
    print_davis();
    // Opening shell.
    if ((purse = malloc(sizeof(struct Purse))) == 0) {
        warn("Could not initialize purse.");
        return FAILURE;
    }

    curr_cmd = malloc(sizeof(struct Node));

    if (initialize_history() == FAILURE) {
        warn("Could not initialize history.");
        return FAILURE;
    }

    davis();
    return 0;
}

void davis()
{
    printf("Hey, I'm DAVIS. How may I assist You?\n");
    if (LOGGING == 1) {
        purse->points = 10000;
    } else {
        purse->points = 100;
    }
    shell_running = 1;
    while (shell_running) {
        get_input(); // ls q w -l e -o -p
        parse_input_into_commands(); // korrekt geparst
        print_arguments();
        sort_flags_in_arguments(in->cmd_one);
        print_arguments();
        chain_up_flags(in->cmd_one);
        print_arguments();
        put_flags_first(in->cmd_one);
        print_arguments();
        exec_command();
    }
    end_davis();
}

void print_davis()
{
    printf("░▒▓███████▓▒░   ░▒▓██████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░  ░▒▓███████▓▒░ \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░ ░▒▓█▓▒░        \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░  ░▒▓█▓▒▒▓█▓▒░  ░▒▓█▓▒░ ░▒▓█▓▒░        \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓████████▓▒░  ░▒▓█▓▒▒▓█▓▒░  ░▒▓█▓▒░  ░▒▓██████▓▒░  \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▓█▓▒░   ░▒▓█▓▒░        ░▒▓█▓▒░ \n");
    printf("░▒▓█▓▒░░▒▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░   ░▒▓█▓▓█▓▒░   ░▒▓█▓▒░        ░▒▓█▓▒░ \n");
    printf("░▒▓███████▓▒░  ░▒▓█▓▒░░▒▓█▓▒░    ░▒▓██▓▒░    ░▒▓█▓▒░ ░▒▓███████▓▒░  \n");
    printf("\n");
}

/*
 * This method takes user input.
 */
void get_input() {
    printf("[DAVIS]\t");
    fflush(stdout);
    input = calloc(MAX_INPUT_BUFFER + 1, sizeof(char));
    pthread_mutex_lock(&input_mutex);

    enable_raw_mode();

    int index = 0;
    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) == -1) continue;

        if (c == '\n' || c == '\r') {
            input[index] = '\0';
            break;
        } else if (c == 127) { // Handle backspace
            if (index > 0) {
                index--;
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == '\033') { // Handle escape sequences
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == -1) continue;
            if (read(STDIN_FILENO, &seq[1], 1) == -1) continue;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': // up
                        clear_line(index);
                        index = traverse_hist(1, curr_cmd, input, &history_command);
                        if (index >= 0) {
                            if (history_command != -1) {
                                curr_cmd = curr_cmd->next;
                            }
                            history_command += 1;
                            printf("%s", input);
                            fflush(stdout);
                        } else {
                            history_command = index;
                            index = 0;
                        }
                        break;
                    case 'B': // down
                        //printf("\r[DAVIS]\tDown arrow pressed\n[DAVIS]\t");
                        clear_line(index);
                        index = traverse_hist(-1, curr_cmd, input, &history_command);
                        if (index >= 0) {
                            if (history_command != -2) {
                                curr_cmd = curr_cmd->prev;
                            }
                            printf("%s", input);
                            fflush(stdout);
                            history_command -= 1;
                        } else {
                            history_command = index;
                            index = 0;
                        }
                        break;
                }
            }
        } else if (c >= 32 && c <= 126) { // Printable characters
            input[index++] = c;
            putchar(c);
            fflush(stdout);
        }
    }

    printf("\n");
    disable_raw_mode();

    pthread_mutex_unlock(&input_mutex);
    in = malloc(sizeof(struct Input));
    if (in == NULL) {
        fprintf(stderr, "Could not allocate memory for struct Input\n");
        exit(1);
    }
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        in->cmd_one[i] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char));
        in->cmd_one[i][0] = '\0';
        in->cmd_two[i] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char));
        in->cmd_two[i][0] = '\0';
    }
    in->no_commands = 0;
    second_cmd_procedure = 0;
}

/*
 * Translating the given input in syntactically correct parameters.
 */
void parse_input_into_commands() {
    int arg_counter = 0;
    char *input_copy = strdup(input); // Kopie der Eingabe machen
    char *pipe_token = strchr(input_copy, '|');
    char *saveptr;
    if (pipe_token != NULL) {
        LOGGER("Parsing()", "Command with pipe.");
        char *first_half = strtok(input_copy, "|");
        char *second_half = strtok(NULL, "|");
        while (*second_half == ' ') { // clear spaces
            second_half++;
        }
        LOGGER("parse(): first half: ", first_half);
        LOGGER("parse(): second half: ", second_half);

        char *tmp = strtok_r(first_half, " ", &saveptr);
        while (tmp != NULL) {
            if (arg_counter >= MAX_INPUT_COUNT - 1) {
                warn("You exceeded argument count on the first command!");
                break;
            }
            strncpy(in->cmd_one[arg_counter], tmp, MAX_ARG_LENGTH);
            in->cmd_one[arg_counter][MAX_ARG_LENGTH] = '\0';
            arg_counter++;
            tmp = strtok_r(NULL, " ", &saveptr);
        }
        in->cmd_one[arg_counter] = NULL;
        arg_counter = 0;

        // We have reached the end of the first command
        tmp = strtok_r(second_half, " ", &saveptr);
        while (tmp != NULL) {
            if (arg_counter >= MAX_INPUT_COUNT - 1) {
                warn("You exceeded argument count on the second command!");
                break;
            }
            strncpy(in->cmd_two[arg_counter], tmp, MAX_ARG_LENGTH);
            in->cmd_two[arg_counter][MAX_ARG_LENGTH] = '\0';
            arg_counter++;
            tmp = strtok_r(NULL, " ", &saveptr);
        }
        in->cmd_two[arg_counter] = NULL;
        in->no_commands = 2;
    } else {
        LOGGER("Parsing()", "Command without pipe.");
        char *tmp = strtok(input_copy, " ");
        while (tmp != NULL) {
            if (arg_counter >= MAX_INPUT_COUNT) {
                warn("You exceeded argument count on the command!");
                break;
            }
            strncpy(in->cmd_one[arg_counter], tmp, MAX_ARG_LENGTH);
            in->cmd_one[arg_counter][MAX_ARG_LENGTH] = '\0';
            arg_counter++;
            tmp = strtok(NULL, " ");
        }
        in->cmd_one[arg_counter] = NULL;
        in->no_commands = 1;
    }
    free(input_copy); // free copy of input
    free(input);
    LOGGER("End of parsing.", "");
}

/*
 * Executes the command, previously saved in command originating from input.
 * Commands can be added by elif-statements.
 */
void exec_command() {
    LOGGER("starting in exec_command()",in->cmd_one[0]);
    int executed = 1;
    if (in == NULL || in->cmd_one[0] == NULL) {
        warn("Please enter a valid input.");
        return;
    }

    if (in->no_commands == 2) { // Piping
        LOGGER("Piping: ","Start");
        // command 0 is being executed first.
        int fd[2];
        pid_t p1, p2;
        if (pipe(fd) < 0) {
            warn("Couldn't perform piped command.");
            executed = FAILURE;
            return;
        }
        p1 = fork();
        if (p1 < 0) {
            warn("Problem occurred while forking.");
            executed = FAILURE;
            return;
        }

        if (p1 == 0) {
            // Setting tunnels for communication between the two processes.
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

            LOGGER("execute()","first command doing...");
            if (execvp(in->cmd_one[0], in->cmd_one) < 0) {
                warn("First command failed.");
                printf("Fail %s", in->cmd_one[0]);
                exit(0);
            } else {
                LOGGER("execute_cmd()", "First command done\n");
                exit(SUCCESS);
            }
        } else {
            p2 = fork();
            if (p2 < 0) {
                warn("Problem occurred while forking.");
                //hist_add(in, FAILURE);
                executed = FAILURE;
                return;
            }
            // Child 2 executing...
            // It only needs to read at the read end
            if (p2 == 0) {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                LOGGER("2nd command exe ...", "");
                if (execvp(in->cmd_two[0], in->cmd_two) < 0) {
                    warn("Second command failed.");
                    printf("Fail %s", in->cmd_two[0]);
                    exit(0);
                } else {
                    exit(SUCCESS);
                }
            } else {
                close(fd[0]); // Close both ends of pipe in parent
                close(fd[1]);
                int status;
                waitpid(p1, &status, 0); // Warte auf p1
                if (WIFEXITED(status)) {
                    if (WEXITSTATUS(status) != 0) {
                        executed = 0;
                    }
                } else {
                    executed = 0;
                }
                waitpid(p2, &status, 0); // Warte auf p2
                if (WIFEXITED(status)) {
                    if (WEXITSTATUS(status) != 0) {
                        executed = 0;
                    }
                } else {
                    executed = 0;
                }
            }
        }
    } else {
        // Regular command
        LOGGER("Executing regular command: ", in->cmd_one[0]);
        if (strcmp(in->cmd_one[0], "quit") == 0) {
            quit();
        } else if (strcmp(in->cmd_one[0], "ls") == 0) {
            LOGGER("Calling ls", "");
            executed = ls(in->cmd_one);
        } else if (strcmp(in->cmd_one[0], "help") == 0) {
            LOGGER("Calling help: ", "");
            executed = help();
        } else if (strcmp(in->cmd_one[0], "clear") == 0) {
            LOGGER("Calling clear: ", in->cmd_one[0]);
            executed = clear();
        } else if (strcmp(in->cmd_one[0], "plot") == 0) {
            LOGGER("Calling plot: ", in->cmd_one[0]);
            executed = plot(in->cmd_one);
        } else if (strcmp(in->cmd_one[0], "echo") == 0) {
            LOGGER("Calling echo: ", in->cmd_one[0]);
            executed = echo(in->cmd_one);
        } else if (strcmp(in->cmd_one[0], "cd") == 0) {
            LOGGER("Calling cd: ", in->cmd_one[0]);
            if (in->cmd_one[1] != NULL) {
                if (chdir(in->cmd_one[1]) != 0) {
                    executed = 0;
                }
            } else {
                if (chdir("." ) != 0) {
                    executed = 0;
                }
            }
        } else if (strcmp(in->cmd_one[0], "hist") == 0) {
            LOGGER("Calling hist: ", in->cmd_one[0]);
            executed = hist(in);
            if (executed == 2) { // successful -e command
                LOGGER("exe_command()", "successful prev command");
                exec_command();
                return;
            }
        } else if (strcmp(in->cmd_one[0], "wordle") == 0) {
            LOGGER("Calling wordle: ", in->cmd_one[0]);
            executed = wordle(purse);
        } else if (strcmp(in->cmd_one[0], "points") == 0) {
            LOGGER("Calling points: ", in->cmd_one[0]);
            printf("Your points: %d\n", purse->points);
        } else {
            LOGGER("exe_command()","System command executing ...");
            pid_t sys_cmd = fork();
            int status;
            if (sys_cmd == -1) {
                warn("Forking system command process failed.");
            }
            if (sys_cmd == 0) {
                if (execvp(in->cmd_one[0], in->cmd_one) < 0) {
                    warn("Could not execute command.");
                    exit(-1);
                } else {
                    printf("System command was successful.\n");
                    exit(SUCCESS);
                }
            } else {
                // wait(NULL);
                waitpid(sys_cmd, &status, 0); // Warten auf den Kindprozess und Exit-Status abfangen
                if (WIFEXITED(status)) {
                    if (WEXITSTATUS(status) != 0) {
                        executed = 0;
                    }
                } else {
                    executed = 0;
                    LOGGER("Exe_command()","Child process terminated abnormally\n");
                }
            }
        }
    }
    purse->points += (in->no_commands + executed) * MAX_INPUT_COUNT;
    curr_cmd = hist_add(in, executed);
    if (total_commands >= INT_MAX) {
        total_commands = -1;
    } else {
        total_commands += 1;
    }
    history_command = -1;
    clear_input_struct();
    printf("\n");
}

/*
 * This implements the insertion sort algorithm and pushes flags to the left and rest to the right.
 */
void sort_flags_in_arguments(char **parsed_input)
{
    char *tmp_flags[MAX_INPUT_COUNT];
    for (int t = 0; t < MAX_INPUT_COUNT; t++) {
        tmp_flags[t] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char));
        if (tmp_flags[t] == NULL) {
            warn("Error - sort_flags");
            return;
        }
        tmp_flags[t][0] = '\0';
    }
    int flagCounter = 0;
    LOGGER("Sorting flags ...", "");
    for (int j = 1; j < MAX_INPUT_COUNT; j++) { // first pick flags
        if (parsed_input[j] != NULL) {
            if (parsed_input[j][0] == '-') {
                strncpy(tmp_flags[flagCounter], parsed_input[j], MAX_ARG_LENGTH);
                tmp_flags[flagCounter][MAX_ARG_LENGTH] = '\0';
                parsed_input[j][0] = '\0';
                flagCounter++;
            }
        } else {
            break;
        }
    }
    LOGGER("Sorting", "non-empty");
    for (int j = 1; j < MAX_INPUT_COUNT; j++) { // secondly pick non-empty spots
        if (parsed_input[j] != NULL && parsed_input[j][0] != '\0') {
            strncpy(tmp_flags[flagCounter], parsed_input[j], MAX_ARG_LENGTH);
            tmp_flags[flagCounter][MAX_ARG_LENGTH] = '\0';
            parsed_input[j][0] = '\0';
            flagCounter++;
        }
    }
    LOGGER("Sorting", "fill back up");
    for (int j = 1; j <= flagCounter; j++) { // fill them back in
        if (tmp_flags[j] != NULL) {
            strcpy(parsed_input[j], tmp_flags[j - 1]);
        }
    }
    flagCounter = 0;
    LOGGER("Sorting done.", "");
    for (int t = 0; t < MAX_INPUT_COUNT; t++) {
        free(tmp_flags[t]);
    }
    if (in->no_commands == 2 && second_cmd_procedure == 0) {
        second_cmd_procedure = 1;
        sort_flags_in_arguments(in->cmd_two);
        second_cmd_procedure = 0;
    }
}

/*
 *  Puts every flag in input into one single bigger flag (f.e. -l -a -> -la) and moving rest closer
 */
void chain_up_flags(char **parsed_sorted_input) {
    LOGGER("Chain:", "Start");
    char combined_flags[MAX_INPUT_COUNT * 10];
    combined_flags[0] = '\0';
    int foundFlag = 0;
    for (int j = 1; j < MAX_INPUT_COUNT; j++) {
        if (parsed_sorted_input[j] != NULL && parsed_sorted_input[j][0] == '-') {
            foundFlag++;
            if (foundFlag != 1) {
                strcat(combined_flags, parsed_sorted_input[j] + 1);
                parsed_sorted_input[j][0] = '\0'; // reset current argument
            } else {
                strcat(combined_flags, parsed_sorted_input[j]);
                parsed_sorted_input[j][0] = '\0';
            }
        } else {
            break;
        }
    }
    if (foundFlag != 0) {
        strcpy(parsed_sorted_input[1], combined_flags);
        combined_flags[0] = '\0'; // reset
    }
    LOGGER("Chaining", "Done");
    if (in->no_commands == 2 && second_cmd_procedure == 0) {
        second_cmd_procedure = 1;
        chain_up_flags(in->cmd_two);
        second_cmd_procedure = 0;
    }
}

/**
 * Finalizes the commands.
 * Input: [CMD, -FLAGS, NULL, ... , ARGS, ..., NULL]
 * Output: [CMD, -FLAGS, ARGS, NULL, ...]
 */
void put_flags_first(char **chained_up_flags) {
    LOGGER("put_flags_first()", "start");

    // Count the number of elements
    int num_elements = 0;
    while (chained_up_flags[num_elements] != NULL) {
        num_elements++;
    }

    for (int i = 1; i < num_elements; i++) {
        if (chained_up_flags[i][0] == '\0') {
            int j = i + 1;
            // Find the next non-empty element
            while (j < num_elements && chained_up_flags[j][0] == '\0') {
                j++;
            }
            // If next non-empty element found, move it to current position
            if (j < num_elements) {
                strcpy(chained_up_flags[i], chained_up_flags[j]);
                chained_up_flags[j][0] = '\0';
            } else { // If next element is NULL, set current element to NULL and exit loop
                chained_up_flags[i] = NULL;
                break;
            }
        }
    }

    // Recursively call put_flags_first for nested commands
    if (in->no_commands == 2 && second_cmd_procedure == 0) {
        second_cmd_procedure = 1;
        put_flags_first(in->cmd_two);
        second_cmd_procedure = 0;
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
    if (LOGGING == 0) {
        return;
    }
    LOGGER("print_arguments()", "Start");
    for (int j = 0; j < MAX_INPUT_COUNT; j++) {
        if (in->cmd_one[j] != NULL) {
            printf("%s,\n", in->cmd_one[j]);
        } else {
            printf("Args sind jetzt NULLs.\n");
        }
    }
    printf("Zweiter command:\n");
    for (int j = 0; j < MAX_INPUT_COUNT; j++) {
        if (in->cmd_two[j] != NULL) {
            printf("%s,\n", in->cmd_two[j]);
        } else {
            printf("Args sind jetzt NULLs.\n");
        }
    }
}

void clear_input_struct()
{
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        free(in->cmd_one[i]);
        free(in->cmd_two[i]);
    }
    free(in);
}

void end_davis()
{
    LOGGER("end_davis()", "Start");
    free_tree();
    free(purse);
    pthread_mutex_destroy(&input_mutex);
    LOGGER("end_davis", "End");
}


void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void clear_line(int length)
{
    for (int i = 0; i < length; i++) {
        printf("\b \b");
    }
    memset(input, '\0', MAX_INPUT_BUFFER + 1);
    fflush(stdout);
}