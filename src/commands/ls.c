#include "ls.h"

void ls(int flag, char *path) // l = 1, a = 2, al = 3
{
    struct dirent *directory_entry;
    DIR *directory_content = opendir(path);
    if (!directory_content) {
        if (errno == ENOENT) {
            warn("Directory cannot be reached.");
        }
        exit(EXIT_FAILURE);
    }
    if ((directory_entry = readdir(directory_content)) == NULL) {
        notify("Your current directory is empty.");
        return;
    }
    while (directory_entry != NULL) {
        if (directory_entry->d_name[0] != '.' || flag >= 2) {
            if (flag == 1 || flag == 3) {
                printf("%s\t%d\n", directory_entry->d_name, directory_entry->d_type);
            } else {
                printf("%s\t", directory_entry->d_name);
            }
        }
        directory_entry = readdir(directory_content);
    }
    printf("\n");
}