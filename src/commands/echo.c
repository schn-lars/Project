#include "echo.h"


int echo(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    char* flags = malloc(sizeof(char) * 100);
    memset(flags, 0, 100);
    if (args[1][0] != '-') { // no "-" found -> no flags / input directly at args[1]
        for (int i= 1; i < MAX_ARG_LENGTH && args[i] != NULL; i++) {
            int j = 0;
            if (i != 1) {
                printf(" ");
            }
            while (args[i][j] != '\0') {
                printf("%c", args[i][j]);
                j++;
            }
        }
    }
    free(flags);
    return SUCCESS;
}