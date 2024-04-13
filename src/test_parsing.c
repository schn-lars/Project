#include "test_parsing.h"

struct Input *input;
struct Input *excp;

/**
 * Executes the tests.
 * @return
 */
int main()
{
    malloc_struct();
    if (setup_tests() == 1) {
        printf("Tests successful!\n");
    } else {
        printf("Tests failed.\n");
    }
}

void clear_args()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        input->cmds[i][0] = '\0';
        excp->cmds[i][0] = '\0';
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            input->args[i][j][0] = '\0';
            excp->args[i][j][0] = '\0';
        }
    }
}

void malloc_struct()
{
    input = malloc(sizeof (struct Input));
    excp = malloc(sizeof (struct Input));
    if (excp == NULL || input == NULL) {
        warn("Error malloc");
        exit(1);
    }
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        input->cmds[i] = malloc((MAX_COMMAND_LENGTH + 1) * sizeof(char)); // Speicher für Befehlszeichenketten
        excp->cmds[i] = malloc((MAX_COMMAND_LENGTH + 1) * sizeof(char)); // Speicher für Befehlszeichenketten
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            input->args[i][j] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char)); // Speicher für Argumentzeichenketten
            excp->args[i][j] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char)); // Speicher für Argumentzeichenketten
        }
    }
}

int setup_tests()
{
    strcpy(input->args[0][0], "1");
    strcpy(input->args[0][1], "2");
    strcpy(input->args[0][2], "3");
    strcpy(input->args[1][0], "-3");
    strcpy(input->args[1][1], "o");
    strcpy(input->args[1][2], "NULL");
    strcpy(input->args[1][3], "-l");

    strcpy(excp->args[0][0], "1");
    strcpy(excp->args[0][1], "2");
    strcpy(excp->args[0][2], "3");
    strcpy(excp->args[1][0], "-3l");
    strcpy(excp->args[1][1], "o");
    strcpy(excp->args[1][2], "NULL");

    print_arguments();
    if (execute_test() == 0) {
        return 0;
    }
    clear_args();

    strcpy(input->args[0][0], "test-txt");
    strcpy(input->args[0][1], "plot");
    strcpy(input->args[0][2], "-l");
    strcpy(input->args[0][3], "-l");
    strcpy(input->args[1][0], "Hallo");
    strcpy(input->args[1][1], "test.txt");
    strcpy(input->args[1][2], "-ht");
    strcpy(input->args[1][3], "-p");
    strcpy(input->args[1][4], "Cerberus");

    strcpy(excp->args[0][0], "-ll");
    strcpy(excp->args[0][1], "test-txt");
    strcpy(excp->args[0][2], "plot");
    strcpy(excp->args[1][0], "-htp");
    strcpy(excp->args[1][1], "Hallo");
    strcpy(excp->args[1][2], "test.txt");
    strcpy(excp->args[1][3], "Cerberus");

    print_arguments();
    if (execute_test() == 0) {
        return 0;
    }
    clear_args();

    strcpy(input->args[0][0], "_txt");
    strcpy(input->args[0][1], "-D");
    strcpy(input->args[0][2], "L");
    strcpy(input->args[0][3], "-A");
    strcpy(input->args[0][4], "A");
    strcpy(input->args[0][5], "-V");
    strcpy(input->args[0][6], "-I");
    strcpy(input->args[0][7], "R");
    strcpy(input->args[0][8], "-S");
    strcpy(input->args[0][9], "S");

    strcpy(excp->args[0][0], "-DAVIS");
    strcpy(excp->args[0][1], "_txt");
    strcpy(excp->args[0][2], "L");
    strcpy(excp->args[0][3], "A");
    strcpy(excp->args[0][4], "R");
    strcpy(excp->args[0][5], "S");

    print_arguments();
    if (execute_test() == 0) {
        return 0;
    }
    clear_args();
    free_struct();
    return 1;
}

int execute_test()
{
    sort_flags_in_arguments();
    chain_up_flags();
    put_flags_first();
    int status = compareArrays();
    clear_args();
    return status;
}

void free_struct()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        free(input->cmds[i]);
        free(excp->cmds[i]);
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            free(input->args[i][j]);
            free(excp->args[i][j]);
        }
    }
    free(input);
    free(excp);
}


int compareArrays()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            if (excp->args[i][j] == NULL && input->args[i][j] != NULL) {
                return 0; // Different sizes
            } else if (excp->args[i][j] != NULL && input->args[i][j] == NULL) {
                return 0; // Different sizes
            } else if (excp->args[i][j] != NULL && input->args[i][j] != NULL && strcmp(excp->args[i][j], input->args[i][j]) != 0) {
                return 0; // Different strings
            }
        }
    }
    return 1;
}

/**
 * --------------------------------------------------------------
 *
 *                  COPIED CODE OF DAVIS.C
 *
 * --------------------------------------------------------------
 */
void put_flags_first() // [0] is combined flag, rest may vary
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 1; j < MAX_INPUT_COUNT; j++) {
            if (input->args[i][j][0] == '\0') {
                for (int k = j + 1; k < MAX_INPUT_COUNT; k++) {
                    if (input->args[i][k][0] != '\0') {
                        strcpy(input->args[i][j], input->args[i][k]);
                        input->args[i][k][0] = '\0';
                        break;
                    }
                }
            }
        }
    }
}

void chain_up_flags() {
    char combined_flags[MAX_INPUT_COUNT * 10];
    combined_flags[0] = '\0';

    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        int foundFlag = 0;
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            if (input->args[i][j] != NULL && input->args[i][j][0] == '-') {
                foundFlag++;
                if (j != 0) {
                    strcat(combined_flags, input->args[i][j] + 1);
                    input->args[i][j][0] = '\0'; // reset current argument
                } else {
                    strcat(combined_flags, input->args[i][j]);
                    input->args[i][j][0] = '\0';
                }
            } else {
                break;
            }
        }
        if (foundFlag != 0) {
            strcpy(input->args[i][0], combined_flags);
            combined_flags[0] = '\0'; // reset
        }
    }
}

void sort_flags_in_arguments()
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
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) { // first pick flags
            if (input->args[i][j] != NULL && input->args[i][j][0] != '\0' && input->args[i][j][0] == '-') {
                strcpy(tmp_flags[flagCounter], input->args[i][j]);
                input->args[i][j][0] = '\0';
                flagCounter++;
            }
        }
        for (int j = 0; j < MAX_INPUT_COUNT; j++) { // secondly pick non-empty spots
            if (input->args[i][j][0] != '\0') {
                strcpy(tmp_flags[flagCounter], input->args[i][j]);
                flagCounter++;
            }
        }
        for (int j = 0; j < flagCounter; j++) { // fill them back input
            strcpy(input->args[i][j], tmp_flags[j]);
        }
        flagCounter = 0;
    }
    for (int t = 0; t < MAX_INPUT_COUNT; t++) {
        free(tmp_flags[t]);
    }
}

void print_arguments()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            if ((strcmp(input->args[i][j], "")) != 0) {
                printf("%s,\n", input->args[i][j]);
            } else {
                printf("Input: Args sind jetzt NULLs.\n");
            }
            if ((strcmp(excp->args[i][j], "")) != 0) {
                printf("%s,\n", excp->args[i][j]);
            } else {
                printf("Excp: Args sind jetzt NULLs.\n");
                break;
            }
        }
    }
}
