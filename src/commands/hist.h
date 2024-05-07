/*
 *  hist (FLAG) (NUMBER), effect varies due to following options:
 *      -a => Lists all previous commands
 *      -t => Lists only correctly executed commands
 *      -f => Lists only wrongly executed commands
 *      -e => Executes command with ID = NUMBER
 *      NUMBER => optional integer for desired amount of entries shown (DEFAULT=5)
 */

#ifndef HIST_H
#define HIST_H

#define DEFAULT_COUNT       5
#define RED                 "\x1b[31m"
#define GREEN               "\x1b[32m"
#define BOLD                "\x1b[1m"
#define ITALIC              "\x1b[3m"
#define UNDERLINE           "\x1b[4m"
#define RESET               "\x1b[0m"
#define YELLOW              "\x1b[33m"

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
void hist_add(struct Input *input, int executed); // linked list setup or add nodes
void traverse_hist(int direction); // > 0 goes left, 0 < goes right
struct Node *create_node(struct Input *input, int executed);
void print_history(int executed, int size);
int initialize_history();
void print_red(int number);
void print_green(int number);
void free_tree();
void execute(struct Input *input, int id);

#endif