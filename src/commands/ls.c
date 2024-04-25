#include "ls.h"

int file_name_size;
int readable = 0;

int ls(char **args) // l = 1, a = 2, al = 3
{
    LOGGER("LS", "");
    struct dirent *directory_entry = malloc(sizeof(struct dirent));
    char *flag = malloc(MAX_ARG_LENGTH * sizeof(char));
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

    if (strchr(flag, 'r') != NULL) {
        readable = 1;
    }
    set_longest_name(directory_entry, directory_content);
    display_list_header();
    while (directory_entry != NULL) {
        struct stat *file_stat = malloc(sizeof(struct stat));
        if (stat(directory_entry->d_name, file_stat) == -1) {
            perror("stat");
            return FAILURE;
        }
        if (directory_entry->d_name[0] == '.') {
            if (strchr(flag, 'l') != NULL && strchr(flag, 'a') != NULL) {
                display_as_list(file_stat, directory_entry);
            } else if (strchr(flag, 'a') != NULL) {
                printf("%s\t", directory_entry->d_name);
            }
        } else {
            if (strchr(flag, 'l') != NULL) {
                display_as_list(file_stat, directory_entry);
            } else {
                printf("%s\t", directory_entry->d_name);
            }
        }
        directory_entry = readdir(directory_content);
    }
    printf("\n");
    readable = 0;
    return 1;
}

/**
*   Creates a title for the command used with -l flag.
 *  For better understanding of the generated output.
*/
void display_list_header()
{
    if (readable == 1) {
        printf("Header\n");
    } else {
        printf("Header\n");
    }
}

/**
 * Execution with -l flag.
 * @param stat_file
 */
void display_as_list(struct stat *stat_file, struct dirent *entry)
{
    if (readable == 1) {
        printf("%-*s\t%s\t%-10s\t%s\n",
               file_name_size,
               entry->d_name,
               format_file_size(stat_file->st_size),
               get_type(entry->d_type),
               permissions_to_string(stat_file->st_mode)
               );
    } else {
        printf("%-*s\t%ld\t%u\t%u\n",
               file_name_size,
               entry->d_name,
               stat_file->st_size,
               entry->d_type,
               stat_file->st_mode
               );
    }
}

void set_longest_name(struct dirent *dirent, DIR *directory_content)
{
    while (dirent != NULL) {
        int tmp_len = strlen(dirent->d_name);
        if (tmp_len > file_name_size) {
            file_name_size = tmp_len;
        }
        dirent = readdir(directory_content);
    }
    file_name_size+= 2;
    rewinddir(directory_content);
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

