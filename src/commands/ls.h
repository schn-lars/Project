/*
 *  ls [OPTION], effect varies due to following options:
 *      -l => Displays current directory as a list.
 *      -a => Includes invisible files as well.
 *
 */
#ifndef LS_H
#define LS_H



#include "../davis.h"
#include <dirent.h>
#include <errno.h>

void display_as_list(char *args); // TODO mit flag -l -> File type | Name
int ls(char **args); // flag: -a (ALL), -l (LIST)

#endif