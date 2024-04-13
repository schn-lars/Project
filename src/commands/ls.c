#include "ls.h"

void ls(struct Input *in) // l = 1, a = 2, al = 3
{
    LOGGER("LS", "");
    struct dirent *directory_entry;
    char *flag = malloc(MAX_ARG_LENGTH * sizeof(char)); // Allocate memory for flag
    if (flag == NULL) {
        warn("LS: flag malloc error");
        return;
    }
    flag[0] = '\0'; //
    DIR *directory_content;
    if (in->cmd_one[1][0] == '-') { // first element is the command itself
        strcat(flag, in->cmd_one[1]);
        if (in->cmd_one[1][1] == '\0') { // if only input is "-" as parameter
            directory_content = opendir(".");
        } else {
            directory_content = opendir(in->cmd_one[1]);
        }
    } else {
        if (in->cmd_one[1][0] == '\0') { // no directory has been given
            directory_content = opendir(".");
        } else {
            directory_content = opendir(in->cmd_one[1]);
        }
    }
    if (!directory_content) {
        if (errno == ENOENT) {
            warn("Directory cannot be reached.");
        }
        return;
    }
    if ((directory_entry = readdir(directory_content)) == NULL) {
        notify("Your current directory is empty.");
        return;
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
}