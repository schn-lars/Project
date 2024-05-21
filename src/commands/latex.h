/*
 * latex <PATH/Filename> <template type> {optional flags} {optional arguments}
 * possible arguments:
 *      - title
 *      - author
 *      - date
 * possible flags:
 *      -
 */
#ifndef LATEX_H
#define LATEX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../davis.h"
#include "./plot.h"

int latex(char **args);
int checkFile(const char *path);
char* removeQuotes(char* str);
char* removeSuffix(char* filename, char* suffix);

#endif
