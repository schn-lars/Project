#include "hist.h"

static struct History *history;

int hist(struct Input *input)
{
    LOGGER("History", "Adding entry.");
    if (history->size == 0) {
        printf("There are no tracked commands.\n");
    } else {
        if (input->cmd_one[1] == NULL) {
            print_history(-1, DEFAULT_COUNT);
            return 1;
        }
        int requested_entries = atoi(input->cmd_one[1]); //
        if (input->cmd_one[1][0] != '-') {
            if (requested_entries != 0) {
                print_history(-1, requested_entries);
            } else {
                warn("Wrong usage of hist. Usage hist [-t,f,a]\n");
                return 0;
            }
        } else {
            if (input->cmd_one[2] != NULL) {
                requested_entries = atoi(input->cmd_one[2]);
                if (requested_entries == 0) {
                    warn("Invalid entries count.");
                    return 0;
                }
            } else {
                requested_entries = DEFAULT_COUNT;
            }
            if ((strcmp(input->cmd_one[1], "-t")) == 0) {
                print_history(1, requested_entries);
            } else if ((strcmp(input->cmd_one[1], "-f")) == 0) {
                print_history(0, requested_entries);
            } else if ((strcmp(input->cmd_one[1], "-a")) == 0) {
                print_history(-1, history->size);
            }
        }
    }
    return 1;
}

int initialize_history()
{
    if ((history = malloc(sizeof(struct History)))) {
        return 0;
    }
    return 1;
}

void hist_add(struct Input *input, int executed)
{
    struct Node *newNode = create_node(input, executed);
    LOGGER("hist_add()", "created new Node");
    if (newNode == NULL) {
        LOGGER("History", "Error creating node!");
        return;
    }
    if (history->size == 0) {
        history->head = newNode;
        history->tail = newNode;
    } else {
        newNode->next = history->head;
        history->head->prev = newNode;
        history->head = newNode;
    }
    history->size+=1;
}

void traverse_hist(int direction)
{

}

/*
 * This iterates over history and prints it. Size is per default on 5, if all its just the size.
 */
void print_history(int executed, int size)
{
    LOGGER("print_history()", "start");
    int items = 0;
    if (history->size < size) {
        items = history->size;
    } else {
        items = size;
    }
    struct Node *curr = history->head;
    while (curr != NULL && items >= 0) {
        LOGGER("print_history", curr->cmd_one[0]);
        if (executed == curr->executed || executed == -1) {
            if (curr->executed == 0) {
                print_red(curr->number);
            } else {
                print_green(curr->number);
            }
            printf(" %s ", curr->cmd_one[0]);
            for (int j = 1; j < MAX_INPUT_COUNT && curr->cmd_one[j] != NULL; j++) {
                printf("%s", curr->cmd_one[j]);
                if (curr->cmd_one[j + 1] != NULL) {
                    printf(", ");
                }
            }
            if (curr->no_commands == 2) {
                LOGGER("print_history", "on second command");
                printf(" | %s", curr->cmd_two[0]);
                for (int i = 1; i < MAX_INPUT_COUNT && curr->cmd_two[i] != NULL; i++) {
                    printf("%s", curr->cmd_two[i]);
                    if (curr->cmd_two[i + 1] != NULL) {
                        printf(", ");
                    }
                }
            }
            printf("\n");
        }
        curr = curr->next;
        items = items - 1;
    }
    free(curr);
    LOGGER("print_history()", "printing done");
}

struct Node *create_node(struct Input *input, int executed)
{
    LOGGER("Creating node()", "Start");
    struct Node *node = malloc(sizeof(struct Node));
    if (node == NULL) {
        return NULL;
    }
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        node->cmd_one[i] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char)); // Speicher für Befehlszeichenketten
        node->cmd_one[i][0] = '\0';
        node->cmd_two[i] = malloc((MAX_ARG_LENGTH + 1) * sizeof(char));
        node->cmd_two[i][0] = '\0';
    }
    int j = 0;
    for (j; j < MAX_INPUT_COUNT; j++) {
        if (input->cmd_one[j] != NULL) {
            strcpy(node->cmd_one[j], input->cmd_one[j]);
        } else {
            break;
        }
    }
    if (j <= 29) {
        node->cmd_one[j] = NULL;
    }
    for (j = 0; j < MAX_INPUT_COUNT; j++) {
        if (input->cmd_two[j] != NULL) {
            strcpy(node->cmd_two[j], input->cmd_two[j]);
        } else {
            break;
        }
    }
    if (j <= 29) {
        node->cmd_two[j] = NULL;
    }
    node->no_commands = input->no_commands;
    node->executed = executed;
    node->next = NULL;
    node->prev = NULL;
    node->number = history->size + 1;
    return node;
}

/**
 * Sets the latest node's execution state. Because initially it gets saved to history with executed = 0.
 * @param executed 1 if positive execution (= valid command)
 */
void set_executed(int executed)
{
    if (history->size == 0) {
        warn("History is empty.");
    } else {
        history->head->executed = executed;
    }
}

void print_red(int number)
{
    printf(COLOR_RED   "[%d]"   RESET_COLOR, number);
}

void print_green(int number)
{
    printf(COLOR_GREEN   "[%d]"   RESET_COLOR, number);
}