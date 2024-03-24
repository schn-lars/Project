#ifndef LS_H
#define LS_H

/*
 * Header file for ls-command.
 */

#include "../davis.h"
#include <dirent.h>
#include <errno.h>

void display_as_list(char *args); // TODO mit flag -l -> File type | Name
void ls(int flag_a, char *args);

#endif