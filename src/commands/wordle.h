/*
 *  wordle -> enters the wordle gamemode
 *      RULES:
 *          - You have 6 tries to guess a 5-letter word.
 *          - A letter is displayed ...
 *              ... green if its correct.
 *              ... yellow if it's not yet at correct place.
 *              ... white if letter is not in the solution.
 *
 */

#ifndef WORDLE_H
#define WORDLE_H

#include <time.h>
#include "../davis.h"
#include <unistd.h>

#define WORD_LENGTH     5
#define WORD_COUNT      6
#define WORD_FILE       "../resources/wordle_list.txt"
#define WORDS           2309

struct Word {
     struct Char *word[WORD_COUNT];
};

struct Char {
    char character;
    int correct; // 0 not in word, 1 letter exists, 2 correct position
};

int wordle(struct Purse *purse);
void init_word();
void print_game();
void get_word();
void put_input_into_game();
void set_state();
void print_help();
void hint_true();
void hint_semi();
int max(int a, int b);

#endif