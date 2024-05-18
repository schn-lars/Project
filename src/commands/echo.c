#include "echo.h"

char *color;
char *style;
char *background;

int echo(char **args) {
    if (args[1] == NULL || (args[1][0] == '-' && args[2] == NULL)) {
        warn("Missing data. Usage echo [-FLAGS] <data>");
        return FAILURE;
    }
    set_background(args[1]);
    set_style(args[1]);
    set_color(args[1]);
    if (color != NULL) printf("%s", color);
    if (style != NULL) printf("%s", style);
    if (background != NULL) printf("%s", background);
    for (int i = 1; i < MAX_INPUT_COUNT && args[i] != NULL && args[i][0] != '\0'; i++) {
        if (args[i][0] != '-') {
            printf("%s", args[i]);
            if (args[i + 1] != NULL) printf(" ");
        }
    }
    if (color != NULL || style != NULL || background != NULL) printf("%s", RESET);
    free(background);
    free(color);
    free(style);
    return SUCCESS;
}

void set_style(char *args)
{
    style = calloc(15, sizeof(char));
    if (args != NULL && args[0] == '-') {
        if (strstr(args, "F") != NULL) {
            strcpy(style, STYLE_BOLD);
            return;
        } else if (strstr(args, "I") != NULL) {
            strcpy(style, STYLE_ITALIC);
            return;
        } else if (strstr(args, "U") != NULL) {
            strcpy(style, STYLE_UNDERLINE);
            return;
        }
    }
}

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