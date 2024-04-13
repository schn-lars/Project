#include "hist.h"



struct History *history;

void hist(struct Input *input)
{
    if (history->size == 0) {
        printf("There are no tracked commands.\n");
    } else {
        if ((input->cmd_one[0] != NULL) && ((input->cmd_one[1] != NULL) && (input->cmd_one[1][0] != '-'))) {
            printf("Wrong usage of hist. Usage hist [-t,f,a]");
            hist_add(input, 0);
            return;
        } else if ((strcmp(input->cmd_one[1], "-t")) == 0) {
            print_history(1, DEFAULT_COUNT);
        } else if ((strcmp(input->cmd_one[1], "-f")) == 0) {
            print_history(0, DEFAULT_COUNT);
        } else if ((strcmp(input->cmd_one[1], "-a")) == 0) {
            print_history(-1, history->size);
        }
    }
    hist_add(input, 1);
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
    if (newNode == NULL) {
        LOGGER("History", "Error creating node!");
        return;
    }
    if (history->size == 0) {
        history = malloc(sizeof(struct History));
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
    int items = 0;
    if (history->size < size) {
        items = history->size;
    } else {
        items = size;
    }
    //struct Node *curr = history->head;
    struct Node *curr = malloc(sizeof(struct Node));
    curr = history->head;
    while (curr != NULL && items != 0) {
        if (executed == curr->executed) {
            LOGGER("Hist", "Printing not all.");
            // Todo printen von hist einträgen aufgrund von angegebener executed wert
        } else if (executed == -1) {
            printf("%s | {", curr->cmd_one[0]);
            for (int j = 1; j < MAX_INPUT_COUNT; j++) {
                if (curr->cmd_one[j] != NULL) {
                    printf("%s", curr->cmd_one[j]);
                    if (curr->next != NULL) {
                        printf(", ");
                    }
                } else {
                    if (curr->no_commands == 2) {
                        printf(" | %s", curr->cmd_two[0]);
                        for (int i = 1; i < MAX_INPUT_COUNT; i++) {
                            printf("%s", curr->cmd_two[i]);
                            if (curr->next != NULL) {
                                printf(", ");
                            } else {
                                printf("}\n");
                                break;
                            }
                        }
                    } else {
                        printf("}\n");
                        break;
                    }
                }
            }
            curr = curr->next;
            items--;
        }
    }
    free(curr);
}

struct Node *create_node(struct Input *input, int executed)
{
    struct Node *node = malloc(sizeof(struct Node));
    if (node == NULL) {
        return NULL;
    }
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        strcpy(node->cmd_one[i], input->cmd_one[i]);
        strcpy(node->cmd_two[i], input->cmd_two[i]);
    }
    node->no_commands = input->no_commands;
    node->executed = executed; // TODO maybe set executed later
    node->next = NULL;
    node->prev = NULL;
    return node;
}