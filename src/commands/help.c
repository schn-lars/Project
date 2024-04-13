#include "help.h"

/*
 * Iterates over command folder.
 */
int help() {
    const char *directory = "./commands";
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, ".h") != NULL) {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", directory, ent->d_name);
                print_content_until_sequence(filepath, "*/");
            }
        }
        closedir(dir);
    } else {
        warn("Cannot open directory");
        return 0;
    }
    return 1;
}

/*
 *  Prints title of each header file, which is basically the rules explaining usage of each command.
 */
void print_content_until_sequence(const char *filename, const char *sequence) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file: %s\n", filename);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, sequence) != NULL) {
            fclose(file);
            return;
        }
        printf("%s", line);
    }
    fclose(file);
}