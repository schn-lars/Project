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
    } else {
        if (args[2] == NULL) { // TODO: for loop over args for other arguments
            printf("Missing data.\n");
            return FAILURE;
        }
        memcpy(function, args[2], 65);
    }

    printf("Input: %s \n", function);


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
    }

    // check flags and concatinate to command array

    // here gnuplot command should be ready and will be sent
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (!gnuplotPipe) {
        printf("Error opening Gnuplot.\n");
        return FAILURE;
    }
    fprintf(gnuplotPipe, "%s", command);
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "exit\n");

    pclose(gnuplotPipe);
    return SUCCESS;
    // TODO: Free everything
    return FAILURE;
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
