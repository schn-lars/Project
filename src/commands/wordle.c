#include "wordle.h"

struct Word *game[WORD_COUNT];
char *input_word;
char solution[WORD_LENGTH + 1];
int guesses = 0;
int state = 0;
struct Purse *player_purse;

/**
 * Called when user calls wordle.
 * @param purse The current stash of points he has earned.
 * @return whether he solved it or not
 */
int wordle(struct Purse *purse)
{
    LOGGER("wordle()" , "start of wordle");
    player_purse = purse;
    init_word();
    clear();
    print_game();

    while (state == 0) { // 1 Loss, 2 Victory
        get_word();
    }
    if (state == 1) {
        printf("[WORDLE] ");
        printf(RED "You lost!" RESET);
        printf(" Better luck next time. The word was: ");
        printf(RED "%s.\n" RESET, solution);
        sleep(5);
        clear();
        print_davis();
        return FAILURE;
    } else if (state == 2) {
        printf("[WORDLE]" GREEN  " You won!\n"   RESET);
        sleep(5);
        clear();
        print_davis();
        return SUCCESS;
    }
    clear();
    print_davis();
    return FAILURE;
}

/**
 * Randomly selects a word from the wordle_list.
 */
void init_word() {
    FILE *file = fopen(WORD_FILE, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    srand(time(NULL));  // Seed the random number generator
    int lines = WORDS;
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

/**
 * This method prints out the gameboard of all current guesses.
 */
void print_game()
{
    clear();
    print_help();
    printf("[WORDLE] You have %d points.\n\n", player_purse->points);
    for (int i = 0; i < WORD_COUNT; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            if (game[i]->word[j]->correct == 1) {
                printf(YELLOW   "[ %c ]"   RESET, game[i]->word[j]->character);
            } else if (game[i]->word[j]->correct == 2) {
                printf(GREEN   "[ %c ]"   RESET, game[i]->word[j]->character);
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

/**
 * This method asks the user for a word or command to purchase a hint or get help.
 */
void get_word()
{
    printf("[WORDLE] ");
    while (1) {
        input_word = calloc(WORDLE_BUFFER + 1, sizeof(char));
        if (fgets(input_word, WORDLE_BUFFER, stdin) == NULL) {
            warn("Error reading input.");
            free(input_word);
            return;
        }
        input_word[strcspn(input_word, "\n")] = '\0'; // Entferne das Zeilenumbruchzeichen

        if (strcmp(input_word, "quit") == 0) {
            state = -1;
            free(input_word);
            return;
        } else {
            int word_count = 0;
            char *token = strtok(input_word, " ");
            while (token != NULL) {
                word_count++;
                token = strtok(NULL, " ");
            }
            if ((word_count == 1 && strlen(input_word) == 5) && input_word[0] != '-' && contains_non_letters(input_word) == 0) {
                put_input_into_game();
                set_state();
                free(input_word);
                return;
            } else {
                if (strcmp(input_word, "-help") == 0) {
                    print_help();
                    free(input_word);
                    return;
                } else if (strcmp(input_word, "-true") == 0) {
                    if (player_purse->points >= 200) {
                        hint_true();
                        player_purse->points -= 200;
                    } else {
                        printf("Not enough points! You have: %d points.\n", player_purse->points);
                    }
                } else if (strcmp(input_word, "-semi") == 0) {
                    if (player_purse->points >= 130) {
                        hint_semi();
                        player_purse->points -= 130;
                    } else {
                        printf("Not enough points! You have: %d points.\n", player_purse->points);
                    }
                } else {
                    warn("Invalid input. Please enter a command or a single word with exactly 5 characters.");
                    free(input_word);
                    return;
                }
                set_state();
                free(input_word);
                return;
            }
        }
    }
}

/**
 * Validates a user's input. A word should not contain special characters or anything besides letters.
 * @param input word entered by user
 * @return 0 if there are just letters
 */
int contains_non_letters(char *input) {
    int index = 0;
    while (index < WORDLE_BUFFER && input[index] != '\0') {
        if (!isalpha(input[index])) {
            return 1;
        }
        index++;
    }
    return 0;
}

/**
 * This method puts the validated word of the user inside of the game structure.
 */
void put_input_into_game()
{
    LOGGER("put_input_into_game()", "start");
    for (int k = 0;k < WORD_LENGTH; k++) {
        game[guesses]->word[k]->character = input_word[k];
        game[guesses]->word[k]->correct = 0;
        player_purse->points += 1;
    }
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (input_word[i] == solution[i]) {
            game[guesses]->word[i]->correct = 2;
            player_purse->points += 10;
        } else {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (input_word[i] == solution[j]) { // is character part of the solution?
                    game[guesses]->word[i]->correct = 1;
                    player_purse->points += 5;
                    break;
                }
            }
        }
    }
    guesses++;
    LOGGER("put_input_into_game()", "end");
}

/**
 * This method ends the game if no more guesses are allowed or user guesses word.
 * Otherwise it merely prints out the game.
 */
void set_state()
{
    LOGGER("set_state()", "start");
    int corr_counter = 0;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (game[max(guesses-1,0)]->word[i]->correct == 2) {
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

/**
 * Called when user inputs -help
 */
void print_help()
{
    printf("Enter '-true' for correct letter and position (200 Points).\n");
    printf("Enter '-semi' for correct letter clue (130 Points).\n");
}

/**
 * Called when user inputs -true
 */
void hint_true()
{
    LOGGER("hint_true()", "Start");
    srand(time(NULL));
    int index = rand() % WORD_LENGTH;
    while (game[max(guesses-1, 0)]->word[index]->correct == 2) {
        index = rand() % WORD_LENGTH;
    }
    game[max(guesses-1, 0)]->word[index]->character = solution[index];
    game[max(guesses-1, 0)]->word[index]->correct = 2;
    LOGGER("hint_true()", "End");
    print_game();
}

/**
 * Called when user inputs -semi
 */
void hint_semi()
{
    int correctChars = 0;
    int semiChars = 0;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (game[max(guesses-1,0)]->word[i]->correct == 2) {
            correctChars++;
        }
        if (game[max(guesses-1,0)]->word[i]->correct == 1) {
            semiChars++;
        }
    }
    if (correctChars + semiChars == 5 || correctChars == 4 || semiChars >= 4) {
        player_purse->points += 130;
        warn("You can not use this hint right now.");
        sleep(2);
        return;
    }
    srand(time(NULL));
    int index, charIndex;
    do {
        index = rand() % WORD_LENGTH;
    } while (game[max(guesses-1, 0)]->word[index]->correct != 0);
    do {
        charIndex = rand() % WORD_LENGTH;
    } while (game[max(guesses-1, 0)]->word[charIndex]->correct != 0 && charIndex == index);
    game[max(0, guesses - 1)]->word[index]->character = solution[charIndex];
    if (game[max(guesses-1,0)]->word[index]->character == solution[index]) {
        game[max(guesses-1,0)]->word[index]->correct = 2;
    } else {
        game[max(guesses-1,0)]->word[index]->correct = 1;
    }
    print_game();
}

int max(int a, int b)
{
    if (a > b) {
        return a;
    } else {
        return b;
    }
}