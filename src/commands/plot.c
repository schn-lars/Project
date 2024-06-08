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
    LOGGER("plot", "start");
    darkmode = 1; // set darkmode on per default
    colorChanged = 0; // if the color was changed with an argument
    titleSet = 0; // if the title was set with an argument
    // Allocate memory and check for NULL pointers
    title = calloc(512, sizeof(char));
    if (!title) {
        printf("Memory allocation failed for title.\n");
        return FAILURE;
    }

    function = calloc(FUNC_SIZE, sizeof(char));
    if (!function) {
        printf("Memory allocation failed for function.\n");
        free(title);
        return FAILURE;
    }

    flags = calloc(128, sizeof(char)); // Adjusted size for safety
    if (!flags) {
        printf("Memory allocation failed for flags.\n");
        free(function);
        free(title);
        return FAILURE;
    }

    command = calloc(2048, sizeof(char)); // Increased size for longer commands
    if (!command) {
        printf("Memory allocation failed for command.\n");
        free(flags);
        free(function);
        free(title);
        return FAILURE;
    }

    arguments = calloc(2048, sizeof(char));
    if (!arguments) {
        printf("Memory allocation failed for arguments.\n");
        free(command);
        free(flags);
        free(function);
        free(title);
        return FAILURE;
    }
    strncpy(flags, args[1], 128);

    if (args[1][0] != '-') { // no "-" found -> no flags / input directly at args[1]
        LOGGER("no flags", "start");
        strncpy(function, args[1], FUNC_SIZE);
        function[FUNC_SIZE - 1] = '\0';
        if (checkFunction() == 0) {
            freeMemory();
            return FAILURE;
        }
        int start = 2;
        if (setupArg(args,start) == 0) {
            freeMemory();
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
            freeMemory();
            return FAILURE;
        }
        strncpy(function, args[2], FUNC_SIZE); // take first arg that's not a flag and save it in function
        function[FUNC_SIZE - 1] = '\0';
        if (checkFunction() == 0) {
            freeMemory();
            return FAILURE;
        }
        int start = 3;
        if (setupArg(args,start) == 0) {
            freeMemory();
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
        freeMemory();
        return FAILURE;
    }
    strcat(command, newLine);
    LOGGER("plot command: ", command);
    if (arguments != NULL) {
        fprintf(gnuplotPipe, "%s", arguments);
    }
    fprintf(gnuplotPipe, "%s", command);
    fflush(gnuplotPipe);
    if (args[1][0] == '-' && strstr(flags, "s") != NULL) { // saves picture of graph as png
        char* savecommand = calloc( 512, sizeof(char));
        char* nameToCheck = calloc( 512, sizeof(char));
        strcpy(savecommand, "set term pngcairo\nset output '");
        //TODO: add argument for path to save and check here
        strcpy(nameToCheck, "./");
        if (titleSet != 1 || title == NULL || strstr(title, " ")) { // there is no title, so we name it per Default DAVIS_plot
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

        free(numb);
        free(savecommand);
        free(nameToCheck);
    }
    fprintf(gnuplotPipe, "exit\n");
    pclose(gnuplotPipe);

    // Free everything
    freeMemory();
    return SUCCESS;
}

int checkFlags() {
    LOGGER("checkFlags", "start");
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
        if (strstr(flags, "l") != NULL || strstr(flags, "p") == NULL) {
            char eBars[100] = " with errorlines ";
            strcat(command, eBars);
        } else if (strstr(flags, "p") != NULL) {
            char eBars[100] = " with errorbars ";
            strcat(command, eBars);
        }
    } else if (strstr(flags, "p") != NULL && strstr(flags, "l") == NULL) { // graph with lines
        char points[100] = " w p";
        strcat(command, points);
    } else if (strstr(flags, "l") != NULL && strstr(flags, "p") != NULL) { // with points and lines
        char linepoints[100] = " w lp ";
        strcat(command, linepoints);
    } else if (strstr(flags, "l") != NULL && strstr(flags, "p") == NULL){ // per default with lines
        char lines[100] = " w l ";
        strcat(command, lines);
    }
    if (strstr(flags, "d") != NULL) { // disables darkmode
        darkmode = 0;
    }
    LOGGER("checkFlags", "end");
    return 1;
}

int checkFunction() {
    LOGGER("checkFunction", "start");
    // Check if file exists or if direct function like sin(X)
    char* functionCommand = malloc(sizeof(char) * 513);
    memset(functionCommand, 0, 513);
    if (strstr(function, ".csv") != NULL) {
        convertCSV(function);
    }
    if (checkFile(function)) // file exists -> add it to command with '' around for gnuplot
    {
        strcpy(functionCommand, "plot '");
        strcat(functionCommand, function);
        strcat(functionCommand, quot);
        strncpy(command, functionCommand, 513);
    }
    else if (strstr(function, "'") != NULL || strchr(function, '"') != NULL) // input is a path/file -> add it to command as is
    {
        char* functionToCheck = removeQuotes(function);
        if (checkFile(functionToCheck)) {
            strcpy(functionCommand, "plot ");
            strcat(functionCommand, function);
            strcat(command, functionCommand);
        } else {
            printf("Path does not exist.\n");
            free(functionCommand);
            return FAILURE;
        }

    }
    else if (checkFile(function) == 0) // file does not exist -> must be a direct function (like sin(x)) -> add as is
    {
        strcpy(functionCommand, "plot ");
        strcat(functionCommand, function);
        strcat(command, functionCommand);
    } else {
        printf("file or function does not exist\n");
        free(functionCommand);
        return FAILURE;
    }
    free(functionCommand);
    LOGGER("checkFunction", "end");
    return 1;
}

char* removeQuotes(char *str) {
    int len = strlen(str);
    int i, j;
    if (str == NULL) {
        return NULL;
    }
    char *result = (char *)malloc((len + 1) * sizeof(char));
    if (result == NULL) {
        fprintf(stderr, "could not malloc space\n");
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
    LOGGER("setupArgs", "start");
    int i;
    for (i = start; args[i] != NULL; i++) { // go over rest of arguments if existing
        printf("for-loop %d and args: %s\n", i, args[i]);
        // add effect of argument to command
        if (strstr(args[i], ":") == NULL) {
            printf("%s is not a valid argument and gets ignored.\n", args[i]);
        } else {
            char* arg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
            memcpy(arg, args[i], MAX_ARG_LENGTH + 1);
            arg[MAX_ARG_LENGTH] = '\0';
            LOGGER("setupArgs","nach memcpy");
            if (checkArgs(arg) == 0) {
                free(arg);
                return FAILURE;
            }
            printf("Before freeing arg in setupArg\n");
            free(arg);
            printf("Freed arg in setupArg\n");
        }
    }
    LOGGER("setupArgs", "end");
    return 1;
}

int checkArgs(char* arg) {
    if (arg == NULL) {
        printf("no argument");
        return 0;
    }
    LOGGER("checkArgs", "start");
    LOGGER("checkArgs", arg);
    int i;
    //char* extractedArg = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    //char* extractedInput = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    char* extractedArg;
    char* extractedInput;
    char* token = strtok(arg, ":");
    if (token != NULL) {
        extractedArg = strdup(token);
        token = strtok(NULL, ":");
        printf("exArg1: %s\n", extractedArg);
        if (token != NULL) {
            extractedInput = strdup(token);  // Second part after the colon
            token = strtok(NULL, ":");
            printf("exIn1: %s\n", extractedInput);
            if (token != NULL) { // if xrange:[start:end] is used then there is another token because there are two ":"
                strcat(extractedInput, ":");
                strcat(extractedInput, token);
                printf("exIn2: %s\n", extractedInput);
            }
        } else {
            extractedInput = NULL;  // No second part found
            printf("%s is not a valid argument and gets ignored.\n", arg);
        }
    } else {
        extractedArg = NULL;  // No colon found, so no first part
        extractedInput = NULL;
        printf("%s is not a valid argument and gets ignored.\n", arg);
    }

    if (strstr(extractedInput, "_") != NULL) {
        removeUnderlines(extractedInput);
    }
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
    //free(extractedArg);
    //free(extractedInput);
    LOGGER("checkArgs", "end");
    return 1;
}

void convertCSV(const char *csvFile) {
    if (strstr(function, "'") != NULL || strchr(function, '"') != NULL) {
        char* newFunction = removeQuotes(function);
        strncpy(function, newFunction, FUNC_SIZE);
        function[FUNC_SIZE - 1] = '\0';
    }
    FILE *csv = fopen(csvFile, "r");
    if (csv == NULL) {
        printf("Could not open CSV file %s for reading.\n", csvFile);
        return;
    }
    // Get the home directory from the environment variables
    const char *homeDir = getenv("HOME");
    if (homeDir == NULL) {
        printf("Could not find the HOME environment variable.\n");
        fclose(csv);
        return;
    }

    // Construct the full path to the desktop
    char txtFile[FUNC_SIZE];
    snprintf(txtFile, sizeof(txtFile), "%s/Desktop/%s", homeDir, "DAVIS_file");
    FILE *txt = fopen(txtFile, "w+");
    if (txt == NULL) {
        printf("Could not open TXT file %s for writing.\n", function);
        fclose(csv);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), csv) != NULL) {
        // Replace commas with tabs
        for (char *ptr = line; *ptr != '\0'; ptr++) {
            if (*ptr == ',') {
                *ptr = '\t';
            }
        }
        fputs(line, txt);
    }

    fclose(csv);
    fclose(txt);
    memset(function, 0, FUNC_SIZE);
    strncpy(function, txtFile, FUNC_SIZE);
    function[FUNC_SIZE - 1] = '\0';
    LOGGER("convertCSV function: ", function);
}

void freeMemory() {
    if (function) {
        free(function);
        function = NULL;  // Avoid double free
    }
    if (flags) {
        free(flags);
        flags = NULL;  // Avoid double free
    }
    if (command) {
        free(command);
        command = NULL;  // Avoid double free
    }
    if (arguments) {
        free(arguments);
        arguments = NULL;  // Avoid double free
    }
    if (title) {
        free(title);
        title = NULL;  // Avoid double free
    }
}