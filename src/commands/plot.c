#include "plot.h"

char* function;
char* flags;
char* command;
char* arguments;
char* newLine = "\n";
char* quot = "'";

int plot(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    function = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    flags = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    command = malloc(sizeof(char) * 1024); // maybe needs more space for longer commands
    memset(command, 0, 1024);
    arguments = malloc(sizeof(char) * 1024);
    memset(arguments, 0, 1024);
    memcpy(flags, args[1], 65);

    if (args[1][0] != '-') { // no "-" found -> no flags / input directly at args[1]
        memcpy(function, args[1], 65);
        checkFunction();
        int start = 2;
        setupArg(args,start);
    } else {
        if (args[2] == NULL) {
            printf("Missing data.\n");
            return FAILURE;
        }
        memcpy(function, args[2], 65); // take first arg that's not a flag and save it in function
        checkFunction();
        int start = 3;
        setupArg(args, start);
    }

    // check flags and concatinate to command array

    // here gnuplot command should be ready and will be sent
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (!gnuplotPipe) {
        printf("Error opening Gnuplot.\n");
        return FAILURE;
    }
    strcat(command, newLine);
    if (arguments != NULL) {
        fprintf(gnuplotPipe, "%s", arguments);
    }
    fprintf(gnuplotPipe, "%s", command);
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "exit\n");

    pclose(gnuplotPipe);

    // Free everything
    free(function);
    free(flags);
    free(command);
    free(arguments);
    return SUCCESS;

    return FAILURE;
}

int checkFunction() {
    // Check if file exists or if direct function like sin(X)
    char* functionCommand = malloc(sizeof(char) * 513);
    memset(functionCommand, 0, 513);
    if (checkFile(function)) // file exists -> add it to command with '' around for gnuplot
    {
        strcpy(functionCommand, "plot '");
        strcat(functionCommand, function);
        strcat(functionCommand, quot);
        strcat(command, functionCommand);
        //sprintf(command, "plot '%s' \n", function);
    }
    else if (strstr(function, "'") != NULL || strchr(function, '"') != NULL) // input is a path/file -> add it to command as is
    {
        //TODO: maybe needs a check if input file without '' exists
        strcpy(functionCommand, "plot ");
        strcat(functionCommand, function);
        strcat(command, functionCommand);
        //sprintf(command, "plot %s \n", function);
    }
    else if (checkFile(function) == 0) // file does not exist -> must be a direct function (like sin(x)) -> add as is
    {
        strcpy(functionCommand, "plot ");
        strcat(functionCommand, function);
        strcat(command, functionCommand);
        //sprintf(command, "plot %s \n", function);
    } else {
        printf("file or function does not exist\n");
        return FAILURE;
    }
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

int setupArg(char** args, int start) {
    int i;
    for (i = start; args[i] != NULL; i++) { // go over rest of arguments if existing
        // add effect of argument to command
        if (strstr(args[i], ":") == NULL) {
            printf("invalid arguments\n");
            return FAILURE;
        } else {
            char* arg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
            memcpy(arg, args[i], 65);
            checkArgs(arg);
            free(arg);
        }
    }
    return 1;
}

int checkArgs(char* arg) {
    if (arg == NULL) {
        printf("no argument");
        return 0;
    }

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
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
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
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
    }
    if (strstr(arg, "ylabel") != NULL || strstr(arg, "yl")) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = "set ylabel '";
        strcat(extractedInput, quot);
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
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
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
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
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
    }
    if (strstr(arg, "color") != NULL || strstr(arg, "colour") != NULL) {
        int i;
        for (i = 0; arg[i] != ':'; i++) {
            //printf("%s \n", &arg[i]);
        }
        i++;
        char* extractedInput = &arg[i];
        // now the input for the title should be saved in extractedInput
        char argCommand[100] = " lc rgb '";
        strcat(argCommand, extractedInput);
        strcat(argCommand, quot);
        printf("argCommand in color: %s\n", argCommand);
        strcat(command, argCommand);
        printf("command in color: %s\n", command);
    }
    return 1;
}
