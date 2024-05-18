/*
 * echo <text> {optional flags} -> gives back the given text and prints it out
 * Possible flags are:
 *      -n => prints each word on a new line
 *      -r => prints each word in a different color
 *      -b => prints the text in bold
 *      -c => prints the text in cursive
 *      -u => prints the text with an underline
 */
#ifndef ECHO_H
#define ECHO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../davis.h"

int echo(char** args);

#endif //ECHO_H
