#include "test_parsing.h"


char *command[MAX_CMD_COUNT][1];
char *arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT];
char *exc[MAX_CMD_COUNT][MAX_INPUT_COUNT];
char *test_arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT];
/**
 * Executes the tests.
 * @return
 */
int main()
{
    if (setup_tests() == 1) {
        printf("Tests successful!\n");
    } else {
        printf("Tests failed.\n");
    }
}

void clear_args()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        command[i][0] = NULL;
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            arguments[i][j] = NULL;
        }
    }
}

int setup_tests()
{
    char *test_arguments1[MAX_CMD_COUNT][MAX_INPUT_COUNT] = {
            {"1", "2", "3", NULL},
            {"-3", "o", "NULL", "-l"},
    };
    char *expected1[MAX_CMD_COUNT][MAX_INPUT_COUNT] = {
            {"1", "2", "3"},
            {"-3l", "o", "NULL", NULL},
    };
    if (execute_test(test_arguments1, expected1) == 0) {
        return 0;
    }
    char *test_arguments2[MAX_CMD_COUNT][MAX_INPUT_COUNT] = {
            {"test-txt", "plot", "-l", "-l"},
            {"Hallo", "test.txt", "-ht", "-p", "Cerberus"},
    };
    char *expected2[MAX_CMD_COUNT][MAX_INPUT_COUNT] = {
            {"-ll", "test-txt", "plot"},
            {"-htp", "Hallo", "test.txt", "Cerberus"},
    };
    if (execute_test(test_arguments2, expected2) == 0) {
        return 0;
    }
    char *test_arguments3[MAX_CMD_COUNT][MAX_INPUT_COUNT] = {
            {"_txt", "-D", "L", "-A", "A","-V", "-I", "R","-S", "S"},
    };
    char *expected3[MAX_CMD_COUNT][MAX_INPUT_COUNT] = {
            {"-DAVIS", "_txt", "L", "A", "R", "S"},
    };
    if (execute_test(test_arguments3, expected3) == 0) {
        return 0;
    }
    return 1;
}

int execute_test(char *test_arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT], char *expected[MAX_CMD_COUNT][MAX_INPUT_COUNT])
{
    copy_arrays(test_arguments, expected);
    sort_flags_in_arguments();
    chain_up_flags();
    put_flags_first();
    int status = compareArrays(exc, arguments);
    clear_args();
    return status;
}

void copy_arrays(char *test_arguments[MAX_CMD_COUNT][MAX_INPUT_COUNT], char *expected[MAX_CMD_COUNT][MAX_INPUT_COUNT]) {
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            arguments[i][j] = test_arguments[i][j];
            exc[i][j] = expected[i][j];
        }
    }
}


int compareArrays(char *exp[MAX_CMD_COUNT][MAX_INPUT_COUNT], char *act[MAX_CMD_COUNT][MAX_INPUT_COUNT])
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            if (exp[i][j] == NULL && act[i][j] != NULL) {
                return 0; // Different sizes
            } else if (exp[i][j] != NULL && act[i][j] == NULL) {
                return 0; // Different sizes
            } else if (exp[i][j] != NULL && act[i][j] != NULL && strcmp(exp[i][j], act[i][j]) != 0) {
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
            if (arguments[i][j] == NULL) {
                for (int k = j + 1; k < MAX_INPUT_COUNT; k++) {
                    if (arguments[i][k] != NULL) {
                        arguments[i][j] = arguments[i][k];
                        arguments[i][k] = NULL;
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
            if (arguments[i][j] != NULL && arguments[i][j][0] == '-') {
                foundFlag++;
                if (j != 0) {
                    strcat(combined_flags, arguments[i][j] + 1);
                    arguments[i][j] = NULL; // reset current argument
                } else {
                    strcat(combined_flags, arguments[i][j]);
                    arguments[i][j] = NULL;
                }
            } else {
                break;
            }
        }
        if (foundFlag != 0) {
            arguments[i][0] = strdup(combined_flags);
            combined_flags[0] = '\0'; // reset
        }
    }
}

void sort_flags_in_arguments()
{
    char *tmp_flags[MAX_INPUT_COUNT];
    for (int t = 0; t < MAX_INPUT_COUNT; t++) {
        tmp_flags[t] = NULL;
    }
    int flagCounter = 0;
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        for (int j = 0; j < MAX_INPUT_COUNT; j++) { // first pick flags
            if ((arguments[i][j] != NULL) && (arguments[i][j][0] == '-')) {
                tmp_flags[flagCounter] = arguments[i][j];
                arguments[i][j] = NULL;
                flagCounter++;
            }
        }
        for (int j = 0; j < MAX_INPUT_COUNT; j++) { // secondly pick non-empty spots
            if ((arguments[i][j] != NULL)) {
                tmp_flags[flagCounter] = arguments[i][j];
                flagCounter++;
            }
        }
        for (int j = 0; j < flagCounter; j++) { // fill them back in
            arguments[i][j] = tmp_flags[j];
        }
        flagCounter = 0;
    }
}

void print_arguments()
{
    for (int i = 0; i < MAX_CMD_COUNT; i++) {
        printf("Command %d: %s\n", i + 1, command[i][0]);
        printf("Arguments for Command %d:\n", i + 1);
        for (int j = 0; j < MAX_INPUT_COUNT; j++) {
            //if (arguments[i][j] != NULL) {
            printf("%s,\n", arguments[i][j]);
            //}
        }
    }
}