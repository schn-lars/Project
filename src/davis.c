#include "davis.h"

/*
 * This is the main access point for our custom shell.
 */

char input[MAX_INPUT_BUFFER];
struct Input *in;
int no_command = 0;
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
        parse_input_into_commands(); // korrekt geparst
        print_arguments();
        sort_flags_in_arguments(in->cmd_one);
        print_arguments();
        chain_up_flags(in->cmd_one);
        print_arguments();
        put_flags_first(in->cmd_one);
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

    // -----
    in = (struct Input *)malloc(sizeof(struct Input));
    if (in == NULL) {
        warn("Could not allocate memory for struct Input");
        exit(1);
    }
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        in->cmd_one[i] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char)); // Speicher für Befehlszeichenketten
        in->cmd_one[i][0] = '\0';
        in->cmd_two[i] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char));
        in->cmd_two[i][0] = '\0';
    }
    in->no_commands = 0;
}



/*
 * Translating the given input in syntactically correct parameters.
 */
void parse_input_into_commands() {
    int arg_counter = 0;
    char *pipe_token = strchr(input, '|');
    if (pipe_token != NULL) {
        LOGGER("Parsing()", "Command with pipe.");
        char *tmp = strtok(pipe_token, " ");
        while (tmp != NULL) {
            if (arg_counter >= MAX_INPUT_COUNT) {
                warn("You exceeded argument count on the first command!");
                break;
            }
            strcpy(in->cmd_one[arg_counter], tmp);
            arg_counter++;
            tmp = strtok(NULL, " "); // anstatt pipe token
        }
        in->cmd_one[arg_counter] = NULL;
        arg_counter = 0;
        // We have reached the end of the first command
        pipe_token = strtok(NULL, "");
        tmp = strtok(pipe_token, " ");
        while (tmp != NULL) {
            if (arg_counter >= MAX_INPUT_COUNT) {
                warn("You exceeded argument count on the second command!");
                break;
            }
            strcpy(in->cmd_two[arg_counter], tmp);
            arg_counter++;
            tmp = strtok(NULL, " "); // anstatt pipe_token
        }
        in->cmd_two[arg_counter]= NULL;
        arg_counter = 0;
        in->no_commands = 2;
    } else {
        LOGGER("Parsing()", "Command without pipe.");
        char *tmp = strtok(input, " ");
        while (tmp != NULL) {
            if (arg_counter >= MAX_INPUT_COUNT) {
                warn("You exceeded argument count on the command!");
                break;
            }
            in->cmd_one[arg_counter] = tmp;
            arg_counter++;
            tmp = strtok(NULL, " ");
        }
        in->cmd_one[arg_counter] = NULL;
        arg_counter = 0;
        in->no_commands = 1;
    }
    LOGGER("End of parsing.", "");
}

/*
 * Executes the command, previously saved in command originating from input.
 * Commands can be added by elif-statements.
 */
void exec_command() {
    LOGGER("starting in exec_command()",in->cmd_one[0]);
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
            if (execvp(in->cmd_one[0], in->cmd_one) < 0) {
                warn("First command failed.");
                printf("%s", in->cmd_one[0]);
                exit(0);
            }
        } else {
            p2 = fork();
            if (p2 < 0) {
                warn("Problem occurred while forking.");
                return;
            }

            // Child 2 executing...
            // It only needs to read at the read end
            if (p2 == 0) {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                if (execvp(in->cmd_two[0], in->cmd_two) < 0) {
                    warn("Second command failed.");
                    printf("%s", in->cmd_two[0]);
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
        LOGGER("Executing regular command: ", in->cmd_one[0]);
        if (strcmp(in->cmd_one[0], "quit") == 0) {
            quit();
        } else if (strcmp(in->cmd_one[0], "ls") == 0) {
            LOGGER("Calling ls", "");
            //ls(in);
        } else if (strcmp(in->cmd_one[0], "help") == 0) {
            LOGGER("Calling help: ", "");
            help();
        } else if (strcmp(in->cmd_one[0], "clear") == 0) {
            LOGGER("Calling clear: ", in->cmd_one[0]);
            clear();
        } else if (strcmp(in->cmd_one[0], "hist") == 0) {
            LOGGER("Calling hist: ", in->cmd_one[0]);
            //hist(in);
        } else {
            warn("Unknown command.");
        }
    }
    in->no_commands = 0;
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
                strcpy(tmp_flags[flagCounter], parsed_input[j]);
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
            strcpy(tmp_flags[flagCounter], parsed_input[j]);
            parsed_input[j][0] = '\0';
            flagCounter++;
        }
    }
    LOGGER("Sorting", "fill back up");
    for (int j = 0; j < flagCounter; j++) {
        printf("tmp_flags[%d] = %s\n", j, tmp_flags[j]);
    }

    for (int j = 1; j < flagCounter; j++) { // fill them back in
        if (tmp_flags[j] != NULL) {
            strcpy(parsed_input[j], tmp_flags[j]);
        }
    }
    flagCounter = 0;
    LOGGER("Sorting done.", "");
    for (int t = 0; t < MAX_INPUT_COUNT; t++) {
        free(tmp_flags[t]);
    }
    if (no_command == 2) {
        sort_flags_in_arguments(in->cmd_two);
    }
}

/*
 *  Puts every flag in input into one single bigger flag (f.e. -l -a -> -la) and moving rest closer
 */
void chain_up_flags(char **parsed_sorted_input) {
    printf("Chaining up.\n");
    char combined_flags[MAX_INPUT_COUNT * 10];
    combined_flags[0] = '\0';
    int foundFlag = 0;
    for (int j = 0; j < MAX_INPUT_COUNT; j++) {
        if (parsed_sorted_input[j] != NULL && parsed_sorted_input[j][0] == '-') {
            foundFlag++;
            if (j != 0) {
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
    if (no_command == 2) {
        chain_up_flags(in->cmd_two);
    }
}

/**
 * Finalizes the commands.
 * Input: [-FLAGS, NULL, ... , ARGS, ..., NULL]
 * Output: [-FLAGS, ARGS, NULL, ...]
 */
void put_flags_first(char **chained_up_flags) // [0] is combined flag, rest may vary
{
    LOGGER("put_flags_first()" , "start");
    for (int j = 1; j < MAX_INPUT_COUNT; j++) {
        if (chained_up_flags[j][0] == '\0') {
            for (int k = j + 1; k < MAX_INPUT_COUNT; k++) {
                if (chained_up_flags[k][0] != '\0') {
                    strcpy(chained_up_flags[j], chained_up_flags[k]);
                    chained_up_flags[k][0] = '\0';
                    break;
                }
            }
        }
    }
}

/*
 * Frees argument array
 */
void cleanup() {
    free(in);
    LOGGER("Freed input", "");
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
    LOGGER("print_arguments()", "Start");
    for (int j = 0; j < MAX_INPUT_COUNT; j++) {
        if (in->cmd_one[j] != NULL) {
            printf("%s,\n", in->cmd_one[j]);
        } else {
            printf("Args sind jetzt NULLs.\n");
        }
    }
    for (int j = 0; j < MAX_INPUT_COUNT; j++) {
        if (in->cmd_two[j] != NULL) {
            printf("%s,\n", in->cmd_two[j]);
        } else {
            printf("Args sind jetzt NULLs.\n");
        }
    }
}

