#include "wordle.h"

struct Word *game[WORD_COUNT];
char input_word[MAX_INPUT_BUFFER + 1];
char solution[WORD_LENGTH + 1];
int guesses = 0;
int state = 0;

int wordle()
{
    LOGGER("wordle()" , "start of wordle");
    init_word();
    print_game();
    while (state == 0) { // 1 Loss, 2 Victory
        get_word();
    }
    if (state == 1) {
        printf("[WORDLE] ");
        printf(COLOR_RED "You lost!" RESET_COLOR);
        printf(" Better luck next time. The word was: ");
        printf(COLOR_RED "%s.\n" RESET_COLOR, solution);
        sleep(5);
    } else if (state == 2) {
        printf("[WORDLE]" COLOR_GREEN  " You won!\n"   RESET_COLOR);
        sleep(5);
    }
    clear();
    print_davis();
    return SUCCESS;
}

void init_word() {
    FILE *file = fopen(WORD_FILE, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    srand(time(NULL));  // Seed the random number generator
    int lines = LINES;
    int random_line = rand() % lines;
    char buffer[WORD_LENGTH + 1];
    int is_null = 1;
    while (is_null == 1) {
        rewind(file);
        random_line = rand() % lines;
        int current_line = 0;
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            if (current_line == random_line) {
                size_t len = strlen(buffer);
                if (len > 0 && buffer[len - 1] == '\n') {
                    buffer[len - 1] = '\0';
                }
                if (buffer[0] != '\0') {
                    strcpy(solution, buffer);
                    solution[WORD_LENGTH] = '\0';
                    is_null = 0;
                    break;
                }
            }
            current_line++;
        }
    }
    if (LOGGING == 1) {
        printf("Solution: %s\n", solution);
    }
    fclose(file);

    for (int i = 0; i < WORD_COUNT; i++) {
        game[i] = malloc(sizeof(struct Word));
        if (game[i] == NULL) {
            fprintf(stderr, "Speicherzuweisung fehlgeschlagen\n");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < WORD_LENGTH; j++) {
            game[i]->word[j] = malloc(sizeof(struct Char));
            if (game[i]->word[j] == NULL) {
                fprintf(stderr, "Speicherzuweisung fehlgeschlagen\n");
                exit(EXIT_FAILURE);
            }
            game[i]->word[j]->character = '\0';
            game[i]->word[j]->correct = 0;
        }
    }
    state = 0;
    guesses = 0;
    LOGGER("init_wordle()", "Initialisation done.");
}

void print_game()
{
    clear();
    for (int i = 0; i < WORD_COUNT; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            if (game[i]->word[j]->correct == 1) {
                printf(COLOR_YELLOW   "[ %c ]"   RESET_COLOR, game[i]->word[j]->character);
            } else if (game[i]->word[j]->correct == 2) {
                printf(COLOR_GREEN   "[ %c ]"   RESET_COLOR, game[i]->word[j]->character);
            } else {
                if (game[i]->word[j]->character == '\0') {
                    printf("[ - ]");
                } else {
                    printf("[ %c ]", game[i]->word[j]->character);
                }
            }
        }
        printf("\n");
    }
}

void get_word()
{
    printf("[WORDLE] ");
    while (1) {
        if (fgets(input_word, MAX_INPUT_BUFFER, stdin) == NULL) {
            warn("Error reading input.");
            return;
        }
        input_word[strcspn(input_word, "\n")] = '\0'; // Entferne das Zeilenumbruchzeichen

        if (strcmp(input_word, "quit") == 0) {
            state = -1;
            return;
        } else {
            // Zähle die Anzahl der Wörter in der Eingabe
            int word_count = 0;
            char *token = strtok(input_word, " ");
            while (token != NULL) {
                word_count++;
                token = strtok(NULL, " ");
            }

            // Überprüfe, ob die Eingabe gültig ist
            if (word_count == 2 || (word_count == 1 && strlen(input_word) == 5)) {
                put_input_into_game();
                set_state();
                return;
            } else {
                warn("Invalid input. Please enter two words or a single word with exactly 5 characters.");
                printf("[WORDLE] ");
            }
        }
    }
}

void put_input_into_game()
{
    LOGGER("put_input_into_game()", "start");
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (input_word[i] == solution[i]) {
            game[guesses]->word[i]->character = input_word[i];
            game[guesses]->word[i]->correct = 2;
        } else {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (input_word[i] == solution[j]) { // is character part of the solution?
                    game[guesses]->word[i]->character = input_word[i];
                    game[guesses]->word[i]->correct = 1;
                }
            }
            if (game[guesses]->word[i]->character == '\0') {
                game[guesses]->word[i]->character = input_word[i];
                game[guesses]->word[i]->correct = 0;
            }
        }
    }
    guesses++;
    LOGGER("put_input_into_game()", "end");
}

void set_state()
{
    LOGGER("set_state()", "start");
    int corr_counter = 0;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (game[guesses-1]->word[i]->correct == 2) {
            corr_counter++;
        }
    }
    if (corr_counter == 5) {
        LOGGER("set_state()", "You won!");
        state = 2;
    } else if (guesses == 6) {
        LOGGER("set_state()", "You lost!");
        state = 1;
    } else {
        print_game();
    }
    LOGGER("set_state()", "end");
}



