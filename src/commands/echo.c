#include "echo.h"

char* output;

int echo(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    output = calloc(2048, sizeof(char));
    int i = 0;
    while (args[i] != NULL) {
        strcat(output, " ");
        strcat(output, args[i]);
    }
    if (output) {
        printf("%s", output);
    } else {
        printf("Unfortunartly there is no input to echo back.");
        return FAILURE;
    }

    return SUCCESS;
}