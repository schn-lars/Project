#include "echo.h"

char *color;
char *style;
char *background;

int echo(char **args) {
    if (args[1] == NULL || (args[1][0] == '-' && args[2] == NULL)) {
        warn("Missing data. Usage echo [-FLAGS] <data>");
        return FAILURE;
    }
    int redirect = check_for_redirect(args);
    if (redirect != -1) { // user wants to redirect input
        for (int j = redirect + 1; j < MAX_INPUT_COUNT && args[j] != NULL; j++) { // iterating over files
            FILE *file = fopen(args[j], "a");
            if (file == NULL) {
                warn("ERROR: Could not create file.");
                return FAILURE;
            } else {
                for (int k = 1; k < redirect; k++) { // printing given input into file
                    if (args[k][0] != '-') {
                        fprintf(file, "%s", args[k]);
                        if (k < redirect - 1) {
                            fprintf(file, " ");
                        }
                    }
                }
                fprintf(file, "\n");
                fclose(file);
            }
        }
        return SUCCESS;
    } else {
        set_background(args[1]);
        set_style(args[1]);
        set_color(args[1]);
        if (color != NULL) printf("%s", color); // printing escape sequence for color ...
        if (style != NULL) printf("%s", style); // .. and style ...
        if (background != NULL) printf("%s", background); // .. and background
        for (int i = 1; i < MAX_INPUT_COUNT && args[i] != NULL && args[i][0] != '\0'; i++) {
            if (args[i][0] != '-') {
                printf("%s", args[i]);
                if (args[i + 1] != NULL) printf(" ");
            }
        }
        if (color != NULL || style != NULL || background != NULL) printf("%s", RESET); // reset if any flags given
        printf("\n");
        free(background);
        free(color);
        free(style);
        return SUCCESS;
    }
}

/**
 * This method sets the style variable, if a suitable flag is given.
 * @param args input[1] containing potential flags
 */
void set_style(char *args)
{
    style = calloc(40, sizeof(char));
    if (args != NULL && args[0] == '-') {
        if (strstr(args, "F") != NULL) {
            strcat(style, STYLE_BOLD);
        }
        if (strstr(args, "I") != NULL) {
            strcat(style, STYLE_ITALIC);
        }
        if (strstr(args, "U") != NULL) {
            strcat(style, STYLE_UNDERLINE);
        }
    }
}

/**
 * This method sets the color variable, if a suitable flag is given.
 * @param args input[1] containing potential flags
 */
void set_color(char *args)
{
    color = calloc(15, sizeof(char));
    if (args != NULL && args[0] == '-') {
        if (strstr(args, "b") != NULL) {
            strcpy(color, COLOR_BLUE);
            return;
        } else if (strstr(args, "d") != NULL) {
            strcpy(color, COLOR_BLACK);
            return;
        } else if (strstr(args, "r") != NULL) {
            strcpy(color, COLOR_RED);
            return;
        } else if (strstr(args, "g") != NULL) {
            strcpy(color, COLOR_GREEN);
            return;
        } else if (strstr(args, "y") != NULL) {
            strcpy(color, COLOR_YELLOW);
            return;
        } else if (strstr(args, "m") != NULL) {
            strcpy(color, COLOR_MAGENTA);
            return;
        } else if (strstr(args, "w") != NULL) {
            strcpy(color, COLOR_WHITE);
            return;
        } else if (strstr(args, "c") != NULL) {
            strcpy(color, COLOR_CYAN);
            return;
        }
    }
}

/**
 * This method sets the background variable, if a suitable flag is given.
 * @param args input[1] containing potential flags
 */
void set_background(char *args)
{
    background = calloc(15, sizeof(char));
    if (args != NULL && args[0] == '-') {
        if (strstr(args, "B") != NULL) {
            strcpy(background, BACKGROUND_BLUE);
            return;
        } else if (strstr(args, "D") != NULL) {
            strcpy(background, BACKGROUND_BLACK);
            return;
        } else if (strstr(args, "R") != NULL) {
            strcpy(background, BACKGROUND_RED);
            return;
        } else if (strstr(args, "G") != NULL) {
            strcpy(background, BACKGROUND_GREEN);
            return;
        } else if (strstr(args, "Y") != NULL) {
            strcpy(background, BACKGROUND_YELLOW);
            return;
        } else if (strstr(args, "M") != NULL) {
            strcpy(background, BACKGROUND_MAGENTA);
            return;
        } else if (strstr(args, "W") != NULL) {
            strcpy(background, BACKGROUND_WHITE);
            return;
        } else if (strstr(args, "C") != NULL) {
            strcpy(background, BACKGROUND_CYAN);
            return;
        }
    }
}

/**
 * Returns the index where ">>" is located in input array.
 * If nothing is found it returns -1
 * @param args entire input
 */
int check_for_redirect(char **args)
{
    for (int i = 1; i < MAX_INPUT_COUNT && args[i] != NULL; i++) {
        if (strcmp(args[i], ">>") == 0) {
            return i;
        }
    }
    return -1;
}