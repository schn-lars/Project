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
    char* arguments = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1) * 7);;

    if (args[1][0] != '-') { // no "-" found -> no flags / input directly at args[1]
        memcpy(function, args[1], 65);
        checkFunction(command, function);
        int start = 2;
        setupArg(args, command, arguments, start);
    } else {
        if (args[2] == NULL) {
            printf("Missing data.\n");
            return FAILURE;
        }
        memcpy(function, args[2], 65); // take first arg that's not a flag and save it in function
        checkFunction(command, function);
        int start = 3;
        setupArg(args, command, arguments, start);
    }

    // check flags and concatinate to command array

    // here gnuplot command should be ready and will be sent
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (!gnuplotPipe) {
        printf("Error opening Gnuplot.\n");
        return FAILURE;
    }
    fprintf(gnuplotPipe, "%s", arguments);
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

int setupArg(char** args, char* command, char* arguments, int start) {
    printf("start setupArg \n");
    int i;
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
    char* newLine = "\n";
    char* quot = "'";
    if (strstr(arg, "title") != NULL || strstr(arg, "tit") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set title '";
        strcat(extractedInput, quot);
        printf("new title: %s \n", extractedInput);
        strcat(argCommand, extractedInput);
        printf("command: %s\n", argCommand);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
        printf("arguments: %s\n", arguments);
    }
    if (strstr(arg, "xlabel") != NULL || strstr(arg, "xl") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set xlabel '";
        strcat(extractedInput, quot);
        printf("new xlabel: %s \n", extractedInput);
        strcat(argCommand, extractedInput);
        printf("command: %s\n", argCommand);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
        printf("arguments: %s\n", arguments);
    }
    if (strstr(arg, "ylabel") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set ylabel '";
        strcat(extractedInput, quot);
        printf("new ylabel: %s \n", extractedInput);
        strcat(argCommand, extractedInput);
        printf("command: %s\n", argCommand);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
        printf("arguments: %s\n", arguments);
    }
    if (strstr(arg, "legend") != NULL || strstr(arg, "box") != NULL || strstr(arg, "key") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100];
        if (strstr(extractedInput, "ne") != NULL || strstr(extractedInput, "northeast") != NULL) {
           memcpy(argCommand, "set key top right\n", 25);
        } else if (strstr(extractedInput, "nw") != NULL || strstr(extractedInput, "northwest") != NULL) {
            memcpy(argCommand, "set key top left\n", 25);
        } else if (strstr(extractedInput, "nc") != NULL || strstr(extractedInput, "north") != NULL) {
            memcpy(argCommand, "set key top center\n", 25);
        } else if (strstr(extractedInput, "se") != NULL || strstr(extractedInput, "southeast") != NULL) {
            memcpy(argCommand, "set key bottom right\n", 25);
        } else if (strstr(extractedInput, "sw") != NULL || strstr(extractedInput, "southwest") != NULL) {
            memcpy(argCommand, "set key bottom left\n", 25);
        } else if (strstr(extractedInput, "sc") != NULL || strstr(extractedInput, "south") != NULL) {
            memcpy(argCommand, "set key bottom center\n", 25);
        }
        strcat(arguments, argCommand);
        printf("arguments: %s\n", arguments);
    }
    if (strstr(arg, "xr") != NULL || strstr(arg, "xrange") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set xr ";
        printf("new x range: %s \n", extractedInput);
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
        printf("arguments: %s\n", arguments);
    }
    if (strstr(arg, "yr") != NULL || strstr(arg, "yrange") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set yr ";
        printf("new y range: %s \n", extractedInput);
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
        printf("arguments: %s\n", arguments);
    }
    if (strstr(arg, "color") != NULL || strstr(arg, "colour") != NULL) {
        // TODO: maybe with set style
    }
    printf("checkArgs success \n");
    return 1;
}
