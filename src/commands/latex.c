#include "latex.h"

char* variables;

int latex(char **args) {
    if (args[1] == NULL || args[2] == NULL) { // args[1] path/filename // args[2] = tempalte type
        warn("Missing data. Usage latex <Path/filename> <template type>");
        return FAILURE;
    }
    // TODO: check if .somethingelse bc thats wrong file
    variables = calloc(sizeof(char), 1024);
    int ind = 3; // from here there could be arguments
    setupLatexArg(args, ind);
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
    FILE *file = fopen(pathToCheck, "w+"); // creates new file
    if (file == NULL) {
        warn("ERROR: Could not create file.\n");
        free(pathToCheck);
        free(variables);
        return FAILURE;
    }
    //checking variables for missing inputs and filling in with default values
    if (strstr(variables, "\\author") == NULL) {
        strcat(variables, "\\newcommand{\\authorname}{<author>}\n");
    }
    if (strstr(variables, "\\course") == NULL) {
        strcat(variables, "\\newcommand{\\course}{<course>}\n");
    }
    if (strstr(variables, "\\homeworkNumber") == NULL && strstr(args[2], "exercise") != NULL) {
        strcat(variables, "\\newcommand{\\homeworkNumber}{<Nr. X>}\n");
    }
    if (strstr(variables, "\\subtitle") == NULL && (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL)) {
        strcat(variables, "\\newcommand{\\subtitlename}{<subtitle>}\n");
    }
    if (strstr(variables, "\\title") == NULL && (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL)) {
        strcat(variables, "\\newcommand{\\titlename}{<Title>}\n");
    }
    if (strstr(variables, "\\uniname") == NULL && (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL)) {
        strcat(variables, "\\newcommand{\\uniname}{<university name>}\n");
    }
    // check which template is chosen
    if (strstr(args[2], "exercise") != NULL) {
        if (copyFileContents("../resources/templates/exercise.tex", file) == FAILURE) {
            fclose(file);
            free(pathToCheck);
            free(variables);
            return FAILURE;
        }
    } else if (strstr(args[2], "report") != NULL || strstr(args[2], "project") != NULL) {
        if (copyFileContents("../resources/templates/project-report.tex", file) == FAILURE) {
            fclose(file);
            free(pathToCheck);
            free(variables);
            return FAILURE;
        }
    }
    free(pathToCheck);
    free(variables);
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

int copyFileContents(const char *sourcePath, FILE *destFile) {
    FILE *sourceFile = fopen(sourcePath, "r");
    int c;
    if (sourceFile == NULL) {
        perror("Could not find the template in DAVIS\n");
        return FAILURE;
    }
    fprintf(destFile, "%s", variables);
    while ((c = fgetc(sourceFile)) != EOF)
    {
        fputc(c, destFile);
    }
    fclose(sourceFile);
    fclose(destFile);
    return SUCCESS;
}

int setupLatexArg(char** args, int start) {
    int i;
    for (i = start; args[i] != NULL; i++) { // go over rest of arguments if existing
        if (strstr(args[i], ":") == NULL) {
            printf("%s is not a valid argument and gets ignored.\n", args[i]);
        } else {
            char* arg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
            memcpy(arg, args[i], 128);
            if (checkLatexArgs(arg) == 0) {
                return FAILURE;
            }
            free(arg);
        }
    }
    return 1;
}

int checkLatexArgs(char* arg) {
    if (arg == NULL) {
        printf("no argument");
        return 0;
    }

    int i;
    char* extractedArg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    for (i = 0; arg[i] != ':'; i++) { // goes over argument until input comes
        extractedArg[i] = arg[i];
    }
    i++;
    extractedArg[i] = '\0';
    if (&arg[i] == NULL) {
        free(extractedArg);
        return 0;
    }
    char* extractedInput = &arg[i];
    if (strstr(extractedArg, "subtitle") != NULL || strstr(extractedArg, "sub") != NULL) {
        strcat(variables, "\\newcommand{\\subtitlename}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "title") != NULL || strstr(extractedArg, "tit") != NULL) {
        strcat(variables, "\\newcommand{\\titlename}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "author") != NULL || strstr(extractedArg, "aut") != NULL) {
        strcat(variables, "\\newcommand{\\authorname}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "semester") != NULL || strstr(extractedArg, "sem") != NULL) {
        strcat(variables, "\\newcommand{\\semester}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "course") != NULL) {
        strcat(variables, "\\newcommand{\\course}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "number") != NULL || strstr(extractedArg, "numb") != NULL) {
        strcat(variables, "\\newcommand{\\homeworkNumber}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else if (strstr(extractedArg, "university") != NULL || strstr(extractedArg, "uni") != NULL) {
        strcat(variables, "\\newcommand{\\uniname}{");
        strcat(variables, extractedInput);
        strcat(variables, "}\n");
    } else {
        printf("%s is not a valid argument and gets ignored.\n", arg);
    }
    free(extractedArg);
    return 1;
}
