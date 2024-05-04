#include "plot.h"


int plot(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    char* function = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
    char* flags = malloc(sizeof(char) * (MAX_ARG_LENGTH + 1));
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


    // Check if file exists or not
    if (checkFile(function))
    {
        printf("Access: File exists at path '%s'\n", function);
        char command[100];
        FILE *gnuplotPipe = popen("gnuplot -persist", "w");
        if (!gnuplotPipe) {
            printf("Error opening Gnuplot.\n");
            return FAILURE;
        }
        printf("IF1 --- plot '%s' with lines\n", function);
        sprintf(command, "plot '%s' with lines\n", function);
        fprintf(gnuplotPipe, "%s", command);
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe, "exit\n");

        pclose(gnuplotPipe);
        return SUCCESS;
    }
    else if (strstr(function, "'") != NULL || strchr(function, '"') != NULL)
    {
        printf("Access: File exists at path '%s'\n", function);
        char command[100];
        FILE *gnuplotPipe = popen("gnuplot -persist", "w");
        if (!gnuplotPipe) {
            printf("Error opening Gnuplot.\n");
            return FAILURE;
        }
        printf("IF2 --- plot %s with lines\n", function);
        sprintf(command, "plot %s with lines\n", function);
        fprintf(gnuplotPipe, "%s", command);
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe, "exit\n");

        pclose(gnuplotPipe);
        return SUCCESS;
    }
    else if (checkFile(function) == 0)
    {
        printf("Access: File does not exists at path '%s'\n", function); // direct function not a file
        char command[100];
        FILE *gnuplotPipe = popen("gnuplot -persist", "w");
        if (!gnuplotPipe) {
            printf("Error opening Gnuplot.\n");
            return FAILURE;
        }
        printf("IF3 --- plot %s with lines\n", function);
        sprintf(command, "plot %s with lines\n", function);
        fprintf(gnuplotPipe, "%s", command);
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe, "exit\n");

        pclose(gnuplotPipe);
        return SUCCESS;
    }

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
