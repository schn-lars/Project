/*
 *  plot <./path/to/data>  {optional flags} {optional arguments} -> plots a given input with the program gnuplot and shows the corresponding graph
 *  Possible flags are:
 *      -b → removes top and right borders
 *      -g → removes grid
 *      -e → adds errorbars (only possible with three columns in data)
 *      -p → graph with points
 *      -l -> graph with lines
 *      -l & -p → graph with lines and points
 *      -d → disables darkmode
 *      -s → saves graph as png directly
 *  Possible arguments (most arguments work with their short form SF or alternativ form AF):
 *  !Write "_" instead of a space. This will be automatically changed back for you in the plot!
 *      title:<title> → sets the title of the graph (SF tit)
 *      xlabel:<xlabel> → sets the horizontal xlabel (SF xl)
 *      ylabel:<ylabel> → sets the vertical ylabel (SF yl)
 *      color:#ffffff → changes color of graph lines/points to hexa value OR (AF colour)
 *      color:red → changes color to corresponding color name (see GNUplot list of colors) (AF colour)
 *      xrange:[<start>:<end>] → limits range of x-values to given input. For example: xr:[-2:2] (SF xr)
 *      yrange:[<start>:<end>] → limits range of y-values to given input. For example: yr:[-2:2] (SF yr)
 *      legend:<position> → moves legend to different corner (AF box, key)
 *          Inputs are: northeast OR ne / northwest OR nw / southeast OR se / southwest OR sw / northcenter OR nc / southcenter OR sc
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#ifndef PLOT_H
#define PLOT_H
#define FUNC_SIZE       1024
#include "../davis.h"
#include "./latex.h"


int plot(char **args);
int checkFlags();
int checkFile(const char *path);
int checkFunction();
char* removeQuotes(char* str);
int checkArgs(char* arg);
int setupArg(char** args, int start);
void freeMemory();
char* removeUnderlines(char* input);
void convertCSV(const char *csvFile);

#endif