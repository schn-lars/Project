/*
 *  ls [OPTION], effect varies due to following options:
 *      -l => Displays current directory as a list.
 *      -a => Includes invisible files as well.
 *      -r => improved readability
 *
 */
#ifndef LS_H
#define LS_H



#include "../davis.h"
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

void set_longest_name(DIR *directory_content, struct dirent *directory_entry);
void display_list_header();
void display_as_list(struct stat *stat_file, struct dirent *entry); // -l
char *get_type(unsigned char type);
char* format_file_size(size_t size);
char *permissions_to_string(mode_t mode);
int ls(char **args); // flag: -a (ALL), -l (LIST)
void print_regular(struct dirent *directory_entry, struct stat *file_stat);

#endif