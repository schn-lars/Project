/*
 *  plot <~/PATH/TO/DATA> -> plots a given input with the GNUplot-library and shows a graph
 *  the settings of a graph are per default:
 *      - with lines
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifndef PLOT_H
#define PLOT_H
#include "../davis.h"


int plot(char **args);
int checkFile(const char *path);

#endif