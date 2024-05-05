#include "plot.h"


int plot(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    char* function = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    char* flags = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    char* command = malloc(sizeof(char) * 100); // maybe needs more space for longer commands
    memcpy(flags, args[1], 65);

    if (args[1][0] != '-') { // no "-" found -> no flags / input directly at args[1]
        memcpy(function, args[1], 65);
        checkFunction(command, function);
        int start = 2;
        setupArg(args, command, start);
    } else {
        if (args[2] == NULL) {
            printf("Missing data.\n");
            return FAILURE;
        }
        memcpy(function, args[2], 65); // take first arg that's not a flag and save it in function
        checkFunction(command, function);
        int start = 3;
        setupArg(args, command, start);
    }

    // check flags and concatinate to command array

    // here gnuplot command should be ready and will be sent
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (!gnuplotPipe) {
        printf("Error opening Gnuplot.\n");
        return FAILURE;
    }
    //fprintf(gnuplotPipe, "set title 'graphtitle'\nset xlabel 'axis'\nset grid\n"); Test if string\nString works
    fprintf(gnuplotPipe, "%s", command);
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "exit\n");

    pclose(gnuplotPipe);
    return SUCCESS;
    // TODO: Free everything
    return FAILURE;
}

int checkFunction(char* command, char *function) {
    // Check if file exists or if direct function like sin(X)
    if (checkFile(function)) // file exists -> add it to command with '' around for gnuplot
    {
        printf("IF1 --- plot '%s'\n", function);
        sprintf(command, "plot '%s' \n", function);
    }
    else if (strstr(function, "'") != NULL || strchr(function, '"') != NULL) // input is a path/file -> add it to command as is
    {
        //TODO: maybe needs a check if input file without '' exists
        printf("IF2 --- plot %s\n", function);
        sprintf(command, "plot %s \n", function);
    }
    else if (checkFile(function) == 0) // file does not exist -> must be a direct function (like sin(x)) -> add as is
    {
        printf("IF3 --- plot %s with lines\n", function);
        sprintf(command, "plot %s \n", function);
    } else {
        printf("file or function does not exist\n");
        return FAILURE;
    }
    printf("checkFunction success \n");
    return 1;
}

/**
 * Function to check whether a file exists or not using
 * access() function. It returns 1 if file exists at
 * given path otherwise returns 0.
 */
int checkFile(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;

    return 1;
}

int setupArg(char** args, char* command, int start) {
    printf("start setupArg \n");
    int i;
    char* arguments; // 7 char* arrays -> 1 array for each possible argument

    for (i = start; args[i] != NULL; i++) { // go over rest of arguments if existing
        // add effect of argument to command
        if (strstr(args[i], ":") == NULL) {
            printf("invalid arguments\n");
            return FAILURE;
        } else {
            char* arg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
            memcpy(arg, args[i], 65);
            checkArgs(arguments, arg);
            free(arg);
        }
    }
    printf("setupArg success \n");
    return 1;
}

int checkArgs(char* arguments, char* arg) {
    printf("start checkArgs \n");
    if (arg == NULL) {
        printf("no argument");
        return 0;
    }
    if (strstr(arg, "title") != NULL) {
        int i = 0;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        int j;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set title ";
        printf("new title: %s \n", extractedInput);
        //char buffer[200];
        strcat(argCommand, extractedInput);
        printf("command: %s\n", argCommand);

    }
    if (strstr(arg, "xlabel") != NULL) {

    }
    if (strstr(arg, "ylabel") != NULL) {

    }
    if (strstr(arg, "legend") != NULL || strstr(arg, "box") != NULL || strstr(arg, "key") != NULL) {

    }
    if (strstr(arg, "xr") != NULL || strstr(arg, "xrange") != NULL) {

    }
    if (strstr(arg, "yr") != NULL || strstr(arg, "yrange") != NULL) {

    }
    if (strstr(arg, "color") != NULL || strstr(arg, "colour") != NULL) {
        // TODO: maybe with set style
    }
    printf("checkArgs success \n");
    return 1;
}
