/*
 *  hist (FLAG) (NUMBER), effect varies due to following options:
 *      -a => Lists all previous commands
 *      -t => Lists only correctly executed commands
 *      -f => Lists only wrongly executed commands
 *      NUMBER => optional integer for desired amount of entries shown (DEFAULT=5)
 */

#ifndef HIST_H
#define HIST_H

#define DEFAULT_COUNT       5
#define COLOR_RED          "\x1b[31m"
#define COLOR_GREEN        "\x1b[32m"
#define STYLE_BOLD         "\x1b[1m"
#define STYLE_ITALIC       "\x1b[3m"
#define STYLE_UNDERLINE    "\x1b[4m"
#define RESET_COLOR        "\x1b[0m"

#include "../davis.h"

struct Node {
    struct Node *prev;
    struct Node *next;
    char *cmd_one[MAX_INPUT_COUNT];
    char *cmd_two[MAX_INPUT_COUNT];
    int no_commands;
    int number;
    int executed;
};

struct History {
    struct Node *head;
    struct Node *tail;
    int size;
};

int hist(struct Input *); // main accesspoint for command
void hist_add(struct Input *, int executed); // linked list setup or add nodes
void traverse_hist(int direction); // > 0 goes left, 0 < goes right
struct Node *create_node(struct Input *, int executed);
void print_history(int executed, int size);
void set_executed(int executed);
int initialize_history();
void print_red(int number);
void print_green(int number);

#endif