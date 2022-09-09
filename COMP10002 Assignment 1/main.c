/* Program to generate term-biased snippets for paragraphs of text.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2022, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: [Enter your full name and student number here before submission]
   Dated:     [Enter the date that you "signed" the declaration]

*/

#include <stdio.h>
#include <string.h>
#include "helper.h"

double getScore(const Strings words, int start, int end, const char *argv[]) {
    double score = 15.0 / (start + 10.0); // + (15 / start + 10) points.
    int key_indexes[MAX_TERMS], last_key_index = 0, key_index;
    clear(key_indexes, MAX_TERMS);

    for (int i = start; i < end; i++) {
        // Check if the word is a query term.
        if ((key_index = isQuery(words[i], argv)) == NO_MATCH) continue;

        // Handles UNIQUE / DUPLICATE query terms.
        for (int j = 0; j < MAX_TERMS; j++) {
            // + (query length / 2) points for every UNIQUE query term.
            if (!key_indexes[j]) {
                score += (double) alNumLen(words[i]) / 2;
                key_indexes[last_key_index++] = key_index;
                break;
            }
            // + 1.0 points for every REPEATED query term.
            if (key_indexes[j] == key_index && (score++)) break;
        }
    }

    // + 0.6 points if the word before is punctuated.
    if (start == 0 || getPunctIndex(words[start - 1])) score += 0.6;
    // + 0.3 points if the ending word is punctuated.
    if (getPunctIndex(words[end - 1])) score += 0.3;
    // - 0.1 point for each word over MIN_SNIPPET_LEN
    score += max((end - start) - MIN_SNIPPET_LEN, 0) * -0.1;

    return score;
}

/**
 * Finds the highest scoring snippet within a paragraph.
 * Loops all possible combos of [start, end], where (end-start)
 * is between MIN and MAX_SNIPPET_LEN.
 *
 * (start == 0) and (end == para_len) handles the case
 * where para_len <= MIN_SNIPPET_LEN.
 * @param words
 * @param para_len
 * @param argv
 * @param start_out
 * @param end_out
 * @param score
 */
void getLowestScore(const Strings words, int para_len, const char *argv[], int *start_out, int *end_out, double *score) {
    int start = 0, end; // Start and End index;
    double cur_score; // Used to temporarily store a score.

    for (; start == 0 || start <= para_len - MIN_SNIPPET_LEN; start++) {
        end = min(para_len, start + MIN_SNIPPET_LEN);

        for (; end == para_len || end - start <= MAX_SNIPPET_LEN; end++) {
            cur_score = getScore(words, start, end, argv);
            if (cur_score > *score) {
                *score = cur_score;
                *start_out = start;
                *end_out = end;
            }
        }
    }
}

void printPara(const Strings f_wrds, int start, int finish, int stg_3) {
    size_t char_count = 0, len, wrap, dots;

    for (int i = start; i < finish; i++) {
        len = strlen(f_wrds[i]) + (i != 0 ? 1 : 0);
        wrap = char_count + len > MAX_OUTPUT_LINE;
        char_count = wrap ? len - 1 : char_count + len;
        dots = stg_3 && i == finish - 1 && need_dots(f_wrds[i]);

        printf("%s%s%s%s", (i != start ? (wrap ? "\n" : " ") : ""), f_wrds[i],
               (dots ? DDOTS : ""), (i == finish - 1 ? "\n" : ""));
    }
}

void formatPara(const Strings words, Strings output, const char *argv[], int *matches) {
    int punct, bold;
    String word, suffix, result;

    for (int i = 0; words[i][0]; i++) {
        strcpy(word, strdup(words[i]));

        bold = isQuery(&word, argv) != NO_MATCH;

        if (bold && (*matches += 1)) {  // Handles bolding words with punctuation
            strcat(suffix, BBOLD);
            if (punct = getPunctIndex(&word), punct != 0) {
                strcpy(&word[punct], "\0"); // Remove punctuation char
                strcat(suffix, &words[i][punct]);  // Add punctuation to suffix
            }
        }

        snprintf(result, sizeof result, "%s%s%s", (bold ? BBOLD : ""), &word, suffix);
        strcpy(output[i], result);

        strcpy(suffix, "");
        strcpy(result, "");
    }
}

int main(int argc, const char *argv[]) {
    Strings words, formatted_words;
    int status, word_count = 0, para_count = 0, matches, start, end;
    double score = 0;

    while (TRUE) {
        while (TRUE) {
            status = get_word(words, &word_count);
            if(status != WORD_FND) break;
            else word_count++;
        }

        formatPara(words, formatted_words, argv, &matches);
        getLowestScore(words, word_count, argv, &start, &end, &score);

        printf("\n======= Stage 1 [para %d; %d words]\n", ++para_count, word_count);

        printf("\n======= Stage 2 [para %d; %d words; %d matches]\n", para_count,
               word_count, matches);
        printPara(formatted_words, 0, word_count, 0);

        printf("\n======= Stage 3 [para %d; start %d; length %d; score %.2lf]\n",
               para_count, start, end - start, score);
        printPara(formatted_words, start, end, 1);

        if(status == EOF) break;

        score = 0, matches = 0, word_count = 0;
        clear(words, MAX_PARA_LEN);
        clear(formatted_words, MAX_PARA_LEN);
    }

    printf("\nta daa!\n");

    return 0;
}