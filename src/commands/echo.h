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

#define RESET              "\x1b[0m"

#define COLOR_BLACK        "\x1b[30m"
#define COLOR_RED          "\x1b[31m"
#define COLOR_GREEN        "\x1b[32m"
#define COLOR_YELLOW       "\x1b[33m"
#define COLOR_BLUE         "\x1b[34m"
#define COLOR_MAGENTA      "\x1b[35m"
#define COLOR_CYAN         "\x1b[36m"
#define COLOR_WHITE        "\x1b[37m"

#define BACKGROUND_BLACK   "\x1b[40m"
#define BACKGROUND_RED     "\x1b[41m"
#define BACKGROUND_GREEN   "\x1b[42m"
#define BACKGROUND_YELLOW  "\x1b[43m"
#define BACKGROUND_BLUE    "\x1b[44m"
#define BACKGROUND_MAGENTA "\x1b[45m"
#define BACKGROUND_CYAN    "\x1b[46m"
#define BACKGROUND_WHITE   "\x1b[47m"

#define STYLE_BOLD         "\x1b[1m"
#define STYLE_ITALIC       "\x1b[3m"
#define STYLE_UNDERLINE    "\x1b[4m"

int echo(char** args);
void set_style(char *args);
void set_color(char *args);
void set_background(char *args);

#endif
