#include "ls.h"

int ls(char **args) // l = 1, a = 2, al = 3
{
    LOGGER("LS", "");
    struct dirent *directory_entry;
    char *flag = malloc(MAX_ARG_LENGTH * sizeof(char)); // Allocate memory for flag
    if (flag == NULL) {
        warn("LS: flag malloc error");
        return 0;
    }
    flag[0] = '\0'; //
    DIR *directory_content;
    if (args[1] != NULL) { // first element is the command itself
        strcat(flag, args[1]);
        if (args[1][0] == '-') { // if only input is "-" as parameter
            directory_content = opendir(".");
        } else {
            directory_content = opendir(args[1]);
        }
    } else {
        directory_content = opendir(".");
    }
    if (!directory_content) {
        if (errno == ENOENT) {
            warn("Directory cannot be reached.");
        }
        return 0;
    }
    if ((directory_entry = readdir(directory_content)) == NULL) {
        notify("Your current directory is empty.");
        return 1;
    }
    while (directory_entry != NULL) {
        if (directory_entry->d_name[0] == '.') {
            if (strchr(flag, 'l') != NULL && strchr(flag, 'a') != NULL) {
                printf("%s\t%d\n", directory_entry->d_name, directory_entry->d_type);
            } else if (strchr(flag, 'a') != NULL) {
                printf("%s\t", directory_entry->d_name);
            }
        } else {
            if (strchr(flag, 'l') != NULL) {
                printf("%s\t%d\n", directory_entry->d_name, directory_entry->d_type);
            } else {
                printf("%s\t", directory_entry->d_name);
            }
        }
        directory_entry = readdir(directory_content);
    }
    printf("\n");
    return 1;
}