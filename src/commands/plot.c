#include "plot.h"

int plot(char **args) {
    if (args[1] == NULL) {
        printf("Missing data.\n");
        return FAILURE;
    }
    const char* filename = args[1];
    char command[100];
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    if (!gnuplotPipe) {
        printf("Error opening Gnuplot.\n");
        return FAILURE;
    }

    sprintf(command, "plot '%s' with lines\n", filename);
    fprintf(gnuplotPipe, "%s", command);
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "exit\n");

    pclose(gnuplotPipe);
    return SUCCESS;
}
