#include "plot.h"

char* function;
char* flags;
char* command;
char* arguments;
char* newLine = "\n";
char* quot = "'";
int colorChanged;
int darkmode;
int titleSet;
char* title;

int plot(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    title = malloc(sizeof(char) * 512);
    darkmode = 1; // set darkmode on per default
    colorChanged = 0; // if the color was changed with an argument
    titleSet = 0; // if the title was set with an argument
    function = malloc(sizeof(char) * 512);
    memset(function, 0, 512);
    flags = malloc(sizeof(char) * 100);
    memset(flags, 0, 100);
    command = malloc(sizeof(char) * 1024); // maybe needs more space for longer commands
    memset(command, 0, 1024);
    arguments = malloc(sizeof(char) * 1024);
    memset(arguments, 0, 1024);
    memcpy(flags, args[1], 100);

    if (args[1][0] != '-') { // no "-" found -> no flags / input directly at args[1]
        memcpy(function, args[1], 512);
        if (checkFunction() == 0) {
            return FAILURE;
        }
        int start = 2;
        if (setupArg(args,start) == 0) {
            return FAILURE;
        }
        // set Default settings
        char lines[100] = " w l ";
        strcat(command, lines);
        char grid[100] = "set grid\n";
        strcat(arguments, grid);
    } else {
        if (args[2] == NULL) {
            printf("Missing data.\n");
            return FAILURE;
        }
        memcpy(function, args[2], 512); // take first arg that's not a flag and save it in function
        if (checkFunction() == 0) {
            return FAILURE;
        }
        int start = 3;
        if (setupArg(args,start) == 0) {
            return FAILURE;
        }
        if (flags != NULL) {
            checkFlags();
        }
    }
    // set green as default color
    if (colorChanged == 0) {
        char argCommand[100] = " lc rgb 'aquamarine' ";
        strcat(command, argCommand);
    }
    if (darkmode == 1) {
        char background[1024] = "set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb 'grey30' behind\nset xlabel tc rgb 'dark-turquoise'\nset ylabel tc rgb 'dark-turquoise'\nset title tc rgb 'dark-turquoise'\nset key tc rgb 'dark-turquoise'\nset tics tc rgb 'dark-turquoise'\nset border lc rgb 'dark-turquoise'\n";
        strcat(arguments, background);
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
    if (args[1][0] == '-' && strstr(flags, "s") != NULL) { // saves picture of graph as png
        char* savecommand = calloc( 512, sizeof(char));
        char* nameToCheck = calloc( 512, sizeof(char));
        strcpy(savecommand, "set term pngcairo\nset output '");
        strcpy(nameToCheck, "./");
        if (titleSet != 1 || title == NULL) { // there is no title, so we name it per Default DAVIS_plot
            strcpy(title, "DAVIS_plot");
        }
        strcpy(nameToCheck, title);
        int i= 0;
        char* numb = calloc(100, sizeof(char));
        while(checkFile(nameToCheck) != 0) {
            char *token = strtok(nameToCheck, "(");
            if (token != NULL) {
                strcpy(nameToCheck, token);
            }
            sprintf(numb,"(%d)", ++i);
            strcat(nameToCheck, numb);
        }
        if (i != 0) {
            sprintf(numb,"(%d)", i);
            strcat(title, numb);
        }
        strcat(savecommand, title);
        strcat(savecommand, "'\nreplot\n");
        fprintf(gnuplotPipe, "%s", savecommand);
        fflush(gnuplotPipe);
        free(savecommand);
        free(nameToCheck);
    }
    fprintf(gnuplotPipe, "exit\n");
    pclose(gnuplotPipe);

    // Free everything
    free(function);
    free(flags);
    free(command);
    free(arguments);
    free(title);
    return SUCCESS;
}

int checkFlags() {
    if (strstr(flags, "b") != NULL) { // removes top and right border of graph
        char border[100] = "set border 3\nset tics nomirror\nset border lw 1.5\n";
        strcat(arguments, border);
    }
    if (strstr(flags, "g") != NULL) { // removes grid
        char grid[100] = "unset grid\n";
        strcat(arguments, grid);
    } else { // per default with grid
        char grid[100] = "set grid\n";
        strcat(arguments, grid);
    }
    if (strstr(flags, "e") != NULL) { // adds errorbars
        //TODO: check if file has 3 columns
        if (strstr(flags, "l") != NULL) {
            char eBars[100] = " with errorlines ";
            strcat(command, eBars);
        } else if (strstr(flags, "p") != NULL) {
            char eBars[100] = " with errorbars ";
            strcat(command, eBars);
        }
    }
    if (strstr(flags, "p") != NULL && strstr(flags, "l") == NULL && strstr(flags, "e") == NULL) { // graph with lines
        char points[100] = " w p";
        strcat(command, points);
    } else if (strstr(flags, "l") != NULL && strstr(flags, "p") != NULL && strstr(flags, "e") == NULL) { // with points and lines
        char linepoints[100] = " w lp ";
        strcat(command, linepoints);
    } else if (strstr(flags, "l") != NULL && strstr(flags, "e") == NULL || strstr(flags, "p") == NULL){ // per default with lines
        char lines[100] = " w l ";
        strcat(command, lines);
    }
    if (strstr(flags, "d") != NULL) { // disables darkmode
        darkmode = 0;
    }
    return 1;
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
        char* functionToCheck = removeQuotes(function);
        if (checkFile(functionToCheck)) {
            strcpy(functionCommand, "plot ");
            strcat(functionCommand, function);
            strcat(command, functionCommand);
            //sprintf(command, "plot %s \n", function);
            free(functionToCheck);
        } else {
            printf("Path does not exist.\n");
            free(functionToCheck);
            return FAILURE;
        }

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

char* removeQuotes(char *str) {
    int len = strlen(str);
    int i, j;
    if (str == NULL) {
        return NULL;
    }
    char *result = (char *)malloc((len + 1) * sizeof(char)); // Speicher für neuen String allozieren
    if (result == NULL) {
        fprintf(stderr, "Speicher konnte nicht alloziert werden\n");
        exit(1);
    }
    for (i = 0, j = 0; i < len; i++) {
        if (str[i] != '"' && str[i] != '\'') {
            result[j++] = str[i];
        }
    }
    result[j] = '\0';
    return result;
}

/**
 * Function to check whether a file exists or not using
 * access() function. It returns 1 if file exists at
 * given path otherwise returns 0.
 */
int checkFile(const char *path)
{
    if (path == NULL) {
        return 0;
    }
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
            if (checkArgs(arg) == 0) {
                return FAILURE;
            }
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
    // now the input for the title should be saved in extractedInput

    if (strstr(extractedArg, "title") != NULL || strstr(extractedArg, "tit") != NULL) {
        char argCommand[100] = "set title '";
        strcpy(title, extractedInput);
        strcat(argCommand, extractedInput);
        strcat(argCommand, quot);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
        titleSet = 1;
    }
    else if (strstr(extractedArg, "xlabel") != NULL || strstr(extractedArg, "xl") != NULL) {
        char argCommand[100] = "set xlabel '";
        strcat(extractedInput, quot);
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
    }
    else if (strstr(extractedArg, "ylabel") != NULL || strstr(extractedArg, "yl")) {
        char argCommand[100] = "set ylabel '";
        strcat(extractedInput, quot);
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
    }
    else if (strstr(extractedArg, "legend") != NULL || strstr(extractedArg, "box") != NULL || strstr(extractedArg, "key") != NULL) {
        char argCommand[100];
        if (strstr(extractedInput, "ne") != NULL || strstr(extractedInput, "northeast") != NULL) {
           memcpy(argCommand, "set key top right\n", 25);
        } else if (strstr(extractedInput, "nw") != NULL || strstr(extractedInput, "northwest") != NULL) {
            memcpy(argCommand, "set key top left\n", 25);
        } else if (strstr(extractedInput, "nc") != NULL || strstr(extractedInput, "northcenter") != NULL) {
            memcpy(argCommand, "set key top center\n", 25);
        } else if (strstr(extractedInput, "se") != NULL || strstr(extractedInput, "southeast") != NULL) {
            memcpy(argCommand, "set key bottom right\n", 25);
        } else if (strstr(extractedInput, "sw") != NULL || strstr(extractedInput, "southwest") != NULL) {
            memcpy(argCommand, "set key bottom left\n", 25);
        } else if (strstr(extractedInput, "sc") != NULL || strstr(extractedInput, "southcenter") != NULL) {
            memcpy(argCommand, "set key bottom center\n", 25);
        }
        strcat(arguments, argCommand);
    }
    else if (strstr(extractedArg, "xr") != NULL || strstr(extractedArg, "xrange") != NULL) {
        char argCommand[100] = "set xr ";
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
    }
    else if (strstr(extractedArg, "yr") != NULL || strstr(extractedArg, "yrange") != NULL) {
        char argCommand[100] = "set yr ";
        strcat(argCommand, extractedInput);
        strcat(argCommand, newLine);
        strcat(arguments, argCommand);
    }
    else if (strstr(extractedArg, "color") != NULL || strstr(extractedArg, "colour") != NULL) {
        char argCommand[100] = " lc rgb '";
        strcat(argCommand, extractedInput);
        strcat(argCommand, quot);
        strcat(command, argCommand);
        colorChanged = 1;
    } else {
        printf("%s is not a valid argument and gets ignored.\n", arg);
    }
    free(extractedArg);
    return 1;
}
