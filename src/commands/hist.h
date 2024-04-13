/*
 *  hist [FLAG], effect varies due to following options:
 *      -a => Lists all previous commands
 *      -t => Lists only correctly executed commands
 *      -f => Lists only wrongly executed commands
 */

#ifndef HIST_H
#define HIST_H

#define DEFAULT_COUNT   5
#include "../davis.h"

struct Node {
    struct Node *prev;
    struct Node *next;
    char *cmd_one[MAX_INPUT_COUNT];
    char *cmd_two[MAX_INPUT_COUNT];
    int no_commands;
    int executed;
};

struct History {
    struct Node *head;
    struct Node *tail;
    int size;
};
void hist(struct Input *); // main accesspoint for command
void hist_add(struct Input *, int executed); // linked list setup or add nodes
void traverse_hist(int direction); // > 0 goes left, 0 < goes right
struct Node *create_node(struct Input *, int executed);
void print_history(int executed, int size);

#endif