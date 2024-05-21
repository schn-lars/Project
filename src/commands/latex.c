#include "latex.h"

int latex(char **args) {
    if (args[1] == NULL || args[2] == NULL) { // args[1] path/filename // args[2] = tempalte type
        warn("Missing data. Usage latex <Path/filename> <template type>");
        return FAILURE;
    }
    // TODO: check if .somethingelse bc thats wrong file

    char* pathToCheck = calloc( 1024, sizeof(char));
    char *suffix = ".tex";
    strcpy(pathToCheck, args[1]);
    if (strstr(pathToCheck, "'") != NULL || strchr(pathToCheck, '"') != NULL) { // check if file has "" or '' and remove them
        char *newToCheck = removeQuotes(pathToCheck);
        strcpy(pathToCheck, newToCheck);
    }
    if (strstr(pathToCheck, suffix) == NULL ) {
        strcat(pathToCheck, suffix); // add suffix to check if it exists with .tex
    }
    if (checkFile(pathToCheck)) { // if there is already a file with this name, add (i)
        printf("This path or file does already exist. I will find you another name for it.\n");
        int i= 0;
        char* numb = calloc(100, sizeof(char));
        while(checkFile(pathToCheck) != 0) {
            char* newPathToCheck = removeSuffix(pathToCheck, suffix); // remove suffix if there is one
            strcpy(pathToCheck, newPathToCheck);
            char *token = strtok(pathToCheck, "(");
            if (token != NULL) {
                strcpy(pathToCheck, token);
            }
            sprintf(numb,"(%d)", ++i);
            strcat(pathToCheck, numb);
            strcat(pathToCheck, suffix); // add suffix to check if it exists with .tex
        }
        free(numb);
    }
    // now pathToCheck contains the next free filename that can be given
    printf("right name: %s", pathToCheck);
    FILE *file = fopen(pathToCheck, "a");
    if (file == NULL) {
        warn("ERROR: Could not create file.");
        return FAILURE;
    }
    free(pathToCheck);
    return SUCCESS;
}

char* removeSuffix(char* filename, char* suffix) {
    if (strstr(filename, suffix) != NULL) { // check if file has .tex suffix and remove it
        size_t lenPath = strlen(filename);
        size_t lenSuffix = strlen(suffix);
        if (lenPath >= lenSuffix && strcmp(filename + lenPath - lenSuffix, suffix) == 0) { // check if pathToCheck end in .tex
            filename[lenPath - lenSuffix] = '\0'; // set \0 instead of .tex
        }
        return filename;
    } else {
        return 0;
    }
}