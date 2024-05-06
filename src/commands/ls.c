#include "ls.h"

int file_name_size = 0;
int file_type_size = 0;
int readable = 0;

int ls(char **args) // l = 1, a = 2, al = 3
{
    LOGGER("LS", "");
    struct dirent *directory_entry;
    char *flag = malloc(MAX_ARG_LENGTH * sizeof(char));
    if (flag == NULL) {
        warn("LS: flag malloc error");
        return 0;
    }
    flag[0] = '\0'; //
    DIR *directory_content;
    char *path = malloc(sizeof(char) * 257);
    memset(path,0,257);
    strcpy(path, ".");
    if (args[1] != NULL) { // first element is the command itself
        if (args[1][0] == '-') { // if only input is "-" as parameter
            strcpy(flag, args[1]);
            if (args[2] != NULL) {
                directory_content = opendir(args[2]);
                strcat(path, "/");
                strcat(path, args[2]);
            } else {
                directory_content = opendir(".");

            }
        } else {
            if (args[2] != NULL) {
                warn("Wrong usage of ls: ls <-FLAGS> <DIRECTORY>");
            } else {
                LOGGER("ls() - 1", "trying to opendir");
                directory_content = opendir(args[1]);
                strcat(path, "/");
                strcat(path, args[1]);
                LOGGER("ls() - 1", "succesful opendir");
            }
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

    if (strchr(flag, 'r') != NULL) {
        readable = 1;
    }
    set_longest_name(directory_content, directory_entry);
    if (strchr(flag, 'l') != NULL) {
        display_list_header();
    }

    while ((directory_entry = readdir(directory_content)) != NULL) {
        struct stat *file_stat = malloc(sizeof(struct stat));
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, directory_entry->d_name);
        if (stat(full_path, file_stat) == -1) {
            perror("stat");
            LOGGER("Stat-ls()", full_path);
            return FAILURE;
        }
        if (directory_entry->d_name[0] == '.') {
            if (strchr(flag, 'l') != NULL && strchr(flag, 'a') != NULL) {
                LOGGER("ls() is al", directory_entry->d_name);
                display_as_list(file_stat, directory_entry);
            } else if (strchr(flag, 'a') != NULL && strchr(flag, 'l') == NULL) {
                LOGGER("ls() in a", directory_entry->d_name);
                print_regular(directory_entry, file_stat);
            }
        } else {
            if (strchr(flag, 'l') != NULL) {
                display_as_list(file_stat, directory_entry);
            } else {
                print_regular(directory_entry, file_stat);
            }
        }
        free(file_stat);
    }
    if (strchr(flag, 'l') == NULL) {
        printf("\n");
    }
    readable = 0;
    file_name_size = 0;
    file_type_size = 0;
    free(flag);
    closedir(directory_content);
    return 1;
}


/**
*   Creates a title for the command used with -l flag.
 *  For better understanding of the generated output.
*/
void display_list_header()
{
    if (readable == 1) {
        printf("\x1b[1m" "%-*s\t%s\t%-*s\t%s\n" "\x1b[0m",
               file_name_size,
               "Name",
               "Size",
               file_type_size,
               "Type",
               "Permissions");
    }
}

/**
 * Execution with -l flag.
 * @param stat_file
 */
void display_as_list(struct stat *stat_file, struct dirent *entry)
{
    if (entry->d_type == DT_DIR) { // is it a directory
        LOGGER("display_as_list() Folder", entry->d_name);
        printf("\x1b[34m" "%-*s\t" "\x1b[0m", file_name_size, entry->d_name);
    } else if (stat_file->st_mode & S_IXUSR || stat_file->st_mode & S_IXGRP || stat_file->st_mode & S_IXOTH) {
        // executeable
        printf("\x1b[32m" "%-*s\t" "\x1b[0m", file_name_size, entry->d_name);
    } else {
        printf("%-*s\t", file_name_size, entry->d_name);
    }
    if (readable == 1) {
        printf("%s\t%-*s\t%s\n",
               format_file_size(stat_file->st_size), // calculates size into readable format
               file_type_size,
               get_type(entry->d_type),
               permissions_to_string(stat_file->st_mode)
               );
    } else {
        printf("%ld\t%u\t%u\n",
               stat_file->st_size,
               entry->d_type,
               stat_file->st_mode
               );
    }
}

void set_longest_name(DIR *directory_content, struct dirent *directory_entry)
{
    while (directory_entry != NULL) {
        int tmp_len = strlen(directory_entry->d_name);
        int temp_type_len = strlen(get_type(directory_entry->d_type));
        if (tmp_len > file_name_size) {
            file_name_size = tmp_len;
        }
        if (temp_type_len > file_type_size) {
            file_type_size = temp_type_len;
        }
        directory_entry = readdir(directory_content);
    }
    rewinddir(directory_content);
    file_name_size += 1;
    file_type_size += 1;

}

char *get_type(unsigned char type)
{
    if (type == DT_DIR) {
        return "Directory";
    } else if (type == DT_REG) {
        return "File";
    } else if (type == DT_LNK) {
        return "Link";
    } else {
        return "Unknown";
    }
}

/**
 * Calculates the proper size of a size for a given input of bytes.
 * @param size
 * @return shortened format of bytes
 */
char* format_file_size(size_t size)
{
    static const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    while (size > 1024 && unit_index < sizeof(units) / sizeof(units[0]) - 1) {
        size /= 1024;
        unit_index++;
    }
    static char result[20]; // Assuming the largest size can be represented in 20 characters
    snprintf(result, sizeof(result), "%ld %s", size, units[unit_index]);
    return result;
}

char *permissions_to_string(mode_t mode)
{
    static char buf[11];
    buf[0] = (S_ISDIR(mode)) ? 'd' : '-';

    // user
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';

    // groups
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';

    // everyone
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';

    buf[10] = '\0';
    return buf;
}

void print_regular(struct dirent *directory_entry, struct stat *file_stat)
{
    if (directory_entry->d_type == DT_DIR) { // is it a directory
        printf("\x1b[34m" "%s  " "\x1b[0m", directory_entry->d_name);
    } else if (file_stat->st_mode & S_IXUSR || file_stat->st_mode & S_IXGRP || file_stat->st_mode & S_IXOTH) {
        // executeable
        printf("\x1b[32m" "%s  " "\x1b[0m", directory_entry->d_name);
    } else {
        printf("%s  ", directory_entry->d_name);
    }
}

