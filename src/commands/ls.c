#include "ls.h"

void ls(int flag_a, char *path)
{
    if (flag_a == 1) { // Retrieves every single file (including . and ..)
        struct dirent *dirent;
        DIR *directory_content = opendir(path);
        if (!directory_content) {
            if (errno == ENOENT) {
                notify("Directory cannot be reached.");
            }
            exit(EXIT_FAILURE);
        }
        while (readdir(directory_content) != NULL && !strcmp(dirent->d_name, ".") && !strcmp(dirent->d_name, "..")) {
            printf("%s", dirent->d_name);
        }
        notify("LS");
    } else {

    }
}