#include "hist.h"

struct History *history;
pthread_mutex_t history_mutex;

int hist(struct Input *input)
{
    LOGGER("History", "Adding entry.");
    if (history->size == 0) {
        printf("There are no tracked commands.\n");
    } else {
        if (input->cmd_one[1] == NULL) {
            print_history(-1, DEFAULT_COUNT);
            return SUCCESS;
        }
        int requested_entries = atoi(input->cmd_one[1]); //
        if (input->cmd_one[1][0] != '-') {
            if (requested_entries != 0) {
                print_history(-1, requested_entries);
            } else {
                warn("Wrong usage of hist. Usage hist [-t,f,a]\n");
                return FAILURE;
            }
        } else {
            if (input->cmd_one[2] != NULL) {
                requested_entries = atoi(input->cmd_one[2]);
                if (requested_entries == 0) {
                    warn("Invalid entries count.");
                    return FAILURE;
                }
            } else {
                requested_entries = DEFAULT_COUNT;
            }
            if ((strcmp(input->cmd_one[1], "-t")) == 0) {
                print_history(1, requested_entries);
            } else if ((strcmp(input->cmd_one[1], "-f")) == 0) {
                print_history(0, requested_entries);
            } else if ((strcmp(input->cmd_one[1], "-a")) == 0) {
                print_history(-1, requested_entries);
            } else if ((strcmp(input->cmd_one[1], "-e")) == 0) {
                if ((atoi(input->cmd_one[2]) <= history->size) && (atoi(input->cmd_one[2]) != 0)) {
                    LOGGER("hist", "executing prev command");
                    execute(input, requested_entries);
                    return 2;
                } else {
                    warn("Your requested number exceeds history!");
                    return FAILURE;
                }
            } else {
                warn("Wrong usage of hist.");
                return FAILURE;
            }
        }
    }
    return SUCCESS;
}

int initialize_history()
{
    if (pthread_mutex_init(&history_mutex, NULL) != 0) {
        warn("History-Mutex initialization failed.");
        return FAILURE;
    }

    if ((history = malloc(sizeof(struct History))) == 0) {
        return FAILURE;
    }
    history->size = 0;
    history->head = NULL;
    history->tail = NULL;
    return SUCCESS;
}

struct Node* hist_add(struct Input *input, int executed)
{
    pthread_mutex_lock(&history_mutex);
    if (history->size >= INT_MAX) {
        free_tree();
        initialize_history();
    }
    struct Node *newNode = create_node(input, executed);
    LOGGER("hist_add()", "created new Node");
    if (newNode == NULL) {
        LOGGER("History", "Error creating node!");
        return history->head;
    }
    if (history->size == 0) {
        history->head = newNode;
        history->tail = newNode;
        newNode->number = 1;
    } else {
        newNode->next = history->head;
        history->head->prev = newNode;
        history->head = newNode;
        newNode->number = history->size + 1;
    }
    history->size+=1;
    pthread_mutex_unlock(&history_mutex);
    return newNode;
}

int traverse_hist(int direction, struct Node *node, char *input, int *current)
{
    LOGGER("traverse()", "start");
    if (node == NULL || history->size == 0) {
        return -1;
    }
    if ((direction == 1 && node->next == NULL && *current != -1) || (*current == -2 && direction == 1)) {
        // no tracked history, no next node if UP, no prev node if DOWN
        LOGGER("traverse()", "no tracked history here");
        return -2;
    } else if ((direction == -1 && node->prev == NULL && *current != -2) || (*current == -1 && direction == -1)) {
        return -1;
    } else {
        LOGGER("traverse()", "valid history");
        struct Node *tmp;
        if (direction == -1) { // going down

            if (*current == -2) {
                tmp = history->tail;
                //printf("Traversing down tail: %d", tmp->number);
            } else {
                tmp = node->prev;
                if (tmp == NULL) {
                    return -1;
                } else {
                    *current = tmp->number;
                    //printf("Traversing down: %d", tmp->number);
                }
            }
        } else {
            if (*current == -1) {
                tmp = history->head;
            } else {
                tmp = node->next;
                if (tmp == NULL) {
                    return -2;
                } else {
                    *current = tmp->number;
                }
            }
        }

        int i;
        int index = 0;
        LOGGER("traverse()", "start iterating");
        for (i = 0; i < MAX_INPUT_COUNT && tmp->cmd_one[i] != NULL; i++) {
            LOGGER("traverse()", "enter loop 1");
            strcat(input, tmp->cmd_one[i]);
            index += strlen(tmp->cmd_one[i]);
            LOGGER("traverse()", "first loop check if ended");
            if (tmp->cmd_one[i+1] != NULL || tmp->no_commands == 2) {
                strcat(input, " ");
                index += 1;
            }
        }
        if (tmp->no_commands == 2) {
            int j = i;
            strcat(input, "|");
            index += 1;
            for (int i = 0; j < MAX_INPUT_COUNT && tmp->cmd_two[i] != NULL; i++) {
                j++;
                strcat(input, " ");
                strcat(input, tmp->cmd_two[i]);
                index += strlen(tmp->cmd_two[i]) + 1;
            }
        }
        return index;
    }
}

/*
 * This iterates over history and prints it. Size is per default on 5, if all its just the size.
 */
void print_history(int executed, int size)
{
    pthread_mutex_lock(&history_mutex);
    LOGGER("print_history()", "start");
    int items = 0;
    if (history->size < size) {
        items = history->size;
    } else {
        items = size;
    }
    struct Node *curr = history->head;
    while (curr != NULL && items > 0) {
        LOGGER("print_history", curr->cmd_one[0]);
        if (executed == curr->executed || executed == -1) {
            if (curr->executed == 0) {
                print_red(curr->number);
            } else {
                print_green(curr->number);
            }
            printf(" %s ", curr->cmd_one[0]);
            for (int j = 1; j < MAX_INPUT_COUNT && curr->cmd_one[j] != NULL; j++) {
                printf("%s ", curr->cmd_one[j]);
            }
            if (curr->no_commands == 2) {
                LOGGER("print_history", "on second command");
                printf("| %s", curr->cmd_two[0]);
                for (int i = 1; i < MAX_INPUT_COUNT && curr->cmd_two[i] != NULL; i++) {
                    printf("%s ", curr->cmd_two[i]);
                }
            }
            printf("\n");
            items = items - 1;
        }
        curr = curr->next;
    }
    LOGGER("print_history()", "printing done");
    pthread_mutex_unlock(&history_mutex);
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
    node->number = 0;
    return node;
}

void print_red(int number)
{
    printf(RED   "[%d]"   RESET, number);
}

void print_green(int number)
{
    printf(GREEN   "[%d]"   RESET, number);
}

void free_tree()
{
    LOGGER("free_tree", "start");
    if (history->size == 0) {
        return;
    } else {
        int counter = 0;
        struct Node *curr = history->head;
        while (curr != NULL) {
            counter++;
            for (int i = 0; i < MAX_INPUT_COUNT; i++) {
                if (curr->cmd_one[i] != NULL) {
                    free(curr->cmd_one[i]);
                }
                if (curr->cmd_two[i] != NULL) {
                    free(curr->cmd_two[i]);
                }
            }
            struct Node *curr_next = curr->next;
            free(curr);
            curr = curr_next;
        }
        if (LOGGING == 1) {
            printf("Deleted %d nodes.\n", counter);
        }
    }
    free(history);
    pthread_mutex_destroy(&history_mutex);
    LOGGER("free_tree", "End");
}

/**
 * Copies the desired command into the current input.
 * @param input
 * @param id
 */
void execute(struct Input *input, int id)
{
    struct Node *curr = history->head;
    while (curr->number != id) {
        curr = curr->next;
    }
    input->no_commands = curr->no_commands;
    for (int i = 0; i < MAX_INPUT_COUNT; i++) {
        if (curr->cmd_one[i] != NULL) {
            input->cmd_one[i] = strdup(curr->cmd_one[i]);
        } else {
            input->cmd_one[i] = NULL;
        }
        if (curr->cmd_two[i] != NULL) {
            input->cmd_two[i] = strdup(curr->cmd_two[i]);
        } else {
            input->cmd_two[i] = NULL;
        }
    }
}