#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"

/* Number of terms to allocate space for initially. */
#define INITIALTERMS 64

/* Number of colour transitions to allocate space for initially. */
#define INITIALTRANSITIONS 16

/* -1 to show the colour hasn't been set. */
#define DEFAULTCOLOUR (-1)
/* -1 to be lower than zero to highlight in case accidentally used. */
#define DEFAULTSCORE (-1)

/* No colour is assigned where no highlighting rules are present. */
#define NO_COLOUR (0)

/* Marker for non-allowed colours. */
#define NONALLOWED (INT_MIN / 2)

/* Amt of Colors */
#define N_COLORS 4

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

/* Calculates WC & CTT Scores */
int get_score(struct problem *p, char* word, int color);
int get_ctt_score(struct problem *p, int prev_c, int curr_c);
void get_max_wc(struct problem *p, char* target, int* max_score, int* max_score_c);
void get_greedy_max(struct problem *p, char* word, int prev_c, int* max_total_score, int* max_total_score_c);

/* Misc Helper Functions */
int valid_color(struct problem *p, char* word, int color);
int index_sc(struct problem *p, int row, int col);

/* Used for Part E & F - Dynamic Programming approach */
struct sc* computeTable(struct problem *p);

/* Returns a word's color score */
int get_score(struct problem *p, char* word, int color) {
    for (int i = 0; i < p->termColourTableCount; i++) {
        struct termColourTable table = p->colourTables[i];
        if (strcmp(word, table.term) != 0) continue; 

        for (int j = 0; j < table.colourCount; j++) {
            if (table.colours[j] == color) return table.scores[j];
        }
    }
    return -1;
}

/* Returns a transition score, given the previous and current color. */
int get_ctt_score(struct problem *p, int prev_c, int curr_c) {
    struct colourTransitionTable *ctt = p->colourTransitionTable;
    for (int i = 0; i < ctt->transitionCount; i++) {
        if (ctt->prevColours[i] == prev_c && ctt->colours[i] == curr_c) {
            return ctt->scores[i];
        }
    }
    return -1;
}

/* Returns the maximum word colour score */
void get_max_wc(struct problem *p, char* target, int* max_score, int* max_score_c) {
    *max_score = 0;

    for (int c = 0; c < N_COLORS; c++) {
        int score = get_score(p, target, c);
        if (score > *max_score) {
            *max_score = score;
            *max_score_c = c;
        }
    }
}

/* Returns the maximum score with a greedy approach */ 
void get_greedy_max(struct problem *p, char* word, int prev_c, int* max_total_score, int* max_total_score_c) {
    get_max_wc(p, word, max_total_score, max_total_score_c);

    for (int c = 0; c < N_COLORS; c++) {
        int ctt_score = get_ctt_score(p, prev_c, c), score = get_score(p, word, c);
        if (score == -1) continue;

        int total_score = ctt_score + score;
        if (total_score > *max_total_score) {
            *max_total_score = total_score;
            *max_total_score_c = c;
        }
    }
}

/* Returns if a color can be used */
int valid_color(struct problem *p, char* word, int color) {
    return get_score(p, word, color) != -1;
}

/* Helper to index 1D array used in computeTable */
int index_sc(struct problem *p, int row, int col) {
    return row * p->termCount + col;
}

/* Dynamic Programming Table used for Part E and F */
struct sc* computeTable(struct problem *p) {
    // Create the C * n array used to cache data.
    int arrSize = N_COLORS * p->termCount;
    struct sc* arr = malloc(arrSize * sizeof (*arr));
    struct sc null_sc = {.prev_c = -1, .score = -1};
    assert(arr != NULL);
    // Set all values to "null"
    for (int i = 0; i < arrSize; i++) arr[i] = null_sc;

    // For each word, iterate all possible colours, and previous
    // possible colors to compute the score.
    for (int i = 0; i < p->termCount; i++) {
        char* term = p->terms[i];
        for (int curr_c = 0; curr_c < N_COLORS; curr_c++) {
            // Can our current color be used?
            if (!valid_color(p, term, curr_c)) continue;
            // For the base case of i = 0, the maximum score is the WC score.
            int max_score = get_score(p, term, curr_c), max_score_prev_c = -1;
            
            // If it's not the base case, check all previous colors.
            for (int prev_c = 0; i != 0 && prev_c < N_COLORS; prev_c++) {
                struct sc prev = arr[index_sc(p, prev_c, i-1)];
                // If the previous score is null, it cannot be used.
                if (prev.score == -1 && prev.prev_c == -1) continue;

                // Score = Previous Score + CTT Score + WC Score
                int total_score = prev.score + get_ctt_score(p, prev_c, curr_c) + get_score(p, term, curr_c);
                if (total_score > max_score) {
                    max_score = total_score;
                    max_score_prev_c = prev_c;
                }
            }

            struct sc entry = {.prev_c = max_score_prev_c, .score = max_score};
            arr[index_sc(p, curr_c, i)] = entry;
        }
    }

    return arr;
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);

    // Loop through each word, finding the maximum WC value.
    for (int i = 0; i < p->termCount; i++) {
        char* word = p->terms[i];
        int max_score = 0, max_score_c = 0;
        get_max_wc(p, word, &max_score, &max_score_c);
        s->termColours[i] = max_score_c;
    }

    return s;
}

struct solution *solveProblemB(struct problem *p){
    struct solution *s = newSolution(p);

    // Finding the maximum score based on a greedy approach.
    for (int i = 0; i < p->termCount; i++) {
        char* word = p->terms[i];
        int max_score = 0, max_score_c = 0;
        // -1 indicates no previous color (i.e. first word)
        int prev_c = (i == 0) ? -1 : s->termColours[i - 1];

        get_greedy_max(p, word, prev_c, &max_score, &max_score_c);
        s->termColours[i] = max_score_c;
    }

    return s;
}

struct solution *solveProblemF(struct problem *p){
    struct solution *s = newSolution(p);
    struct sc* table = computeTable(p), last_sc;
    int termcoloursI = s->termCount - 1, prev_c = -1;

    // iterate through the last column and find the highest score.
    for (int r = 0; r < N_COLORS; r++) {
        last_sc = table[index_sc(p, r, termcoloursI)];
        if (last_sc.score > s->score) {
            s->termColours[termcoloursI] = r;
            s->score = last_sc.score;
            prev_c = last_sc.prev_c;
        }
    }

    // Backtrace using the prev_c of each entry, to find the path taken.
    while (prev_c != -1) {
        s->termColours[--termcoloursI] = prev_c;
        last_sc = table[index_sc(p, prev_c, termcoloursI)];
        prev_c = last_sc.prev_c;
    }

    free(table);
    return s;
}