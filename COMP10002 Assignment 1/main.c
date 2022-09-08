#include <stdio.h>
#include <string.h>

#include "helper.h"

// Constant Values
#define MAX_WORD_LEN 23
#define MAX_PARA_LEN 10000

#define PARA_END 1
#define WORD_FND 2

#define TERM_PUNCT ".,;:!?"
#define NEEDS_DOTS ",;:"
#define PARA_SEPARATOR "\n\n"
#define BBOLD "**"
#define DDOTS "..."
#define TERM_CHARS "\n "
#define BYPASS_PUNCT "-'"

#define MAX_SNIPPET_LEN 30
#define MIN_SNIPPET_LEN 20
#define MAX_OUTPUT_LINE 72

#define MAX_TERMS 50
#define NO_MATCH (-1)

/*
 * Start - Inclusive
 * End - Exclusive
 */
double getScore(const Strings words, int start, int end, const char *argv[]) {
    int key_indexes[MAX_TERMS], last_key_index = 0, key_index;
    double rule1 = 0, rule2 = 0, rule3 = 0, rule4 = 0, rule5 = 0, rule6 = 0;
    clear(key_indexes, MAX_TERMS);

    // ---- 1. Add 15 / (start + 10) points. First word = 0; ----
    rule1 += 15.0 / (start + 10.0);

    for (int i = start; i < end; i++) {
        if ((key_index = isQuery(words[i], argv)) == -1) continue;

        // ---- 3. Add 1.0 points for every other repeated query term ----
        for (int j = 0; argv[j]; j++) {
            if (key_indexes[j] == key_index) {
                rule3++;
                break;
            }
        }

        // ---- 2. Add alpha_len(query) / 2 points for each unique query term ----
        if (rule3 == 0) {
            rule2 += (double)alpha_len(words[i]) / 2;
            key_indexes[last_key_index++] = key_index;
        }
    }

    // ---- 4. Add 0.6 points if the word before is punctuated. ----
    if (start == 0 || getPunctIndex(words[start - 1])) rule4 += 0.6;

    // ---- 5. Add 0.3 points if the ending word is punctuated. ----
    if (getPunctIndex(words[end - 1])) rule5 += 0.3;

    // ---- 6. Subtract 0.1 point for each word over MIN_SNIPPET_LEN ----
    rule6 += max((end - start) - MIN_SNIPPET_LEN, 0) * -0.1;

    double score = rule1 + rule2 + rule3 + rule4 + rule5 + rule6;

    //    printf("[1]: %.1lf, [2]: %.1lf, [3]: %.1lf, [4]: %.1lf, [5]: %.1lf, [6]:
    //    %.1lf, [SCORE]: %.1lf\n", rule1, rule2, rule3, rule4, rule5, rule6,
    //    score);
    return score;
}

void getLowestScore(const Strings words, int len, const char *argv[],
                    int *begin, int *finish, double *score) {
    double tmp_score;
    *score = 0;

    if (len <= MIN_SNIPPET_LEN + 1) {
        *begin = 0;
        *finish = len;
        *score = getScore(words, 0, len, argv);
        return;
    }

    for (int start = 0; start < len - MAX_SNIPPET_LEN; start++) {
        for (int end = min(len, start + MIN_SNIPPET_LEN);
             end - start <= MAX_SNIPPET_LEN; end++) {
            if ((tmp_score = getScore(words, start, end, argv)) > *score) {
                *score = tmp_score;
                *begin = start;
                *finish = end;
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

    for (int i = 0; words[i][0]; i++) {
        String *word = strdup(words[i]), suffix = "", result = "";
        bold = isQuery(*word, argv) != -1;

        if (bold && (*matches += 1)) {  // Handles bolding words with punctuation
            strcat(suffix, BBOLD);
            if (punct = getPunctIndex(*word), punct != 0) {
                removeChar(*word, punct);          // Remove punctuation char
                strcat(suffix, &words[i][punct]);  // Add punctuation to suffix
            }
        }

        snprintf(result, sizeof result, "%s%s%s", (bold ? BBOLD : ""), *word, suffix);
        strcpy(output[i], result);
    }
}

int main(int argc, const char *argv[]) {
    String word;
    Strings words, formatted_words;
    int status = 0, word_count = 0, para_count = 0, matches, start, end;
    double score;

    while (status != EOF) {
        while (status != PARA_END && status != EOF) {
            status = get_word(word, MAX_WORD_LEN);
            strcpy(words[word_count++], word);
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

        status = (status == EOF ? EOF : 0), score = 0, matches = 0, word_count = 0;
        clear(words, MAX_PARA_LEN);
        clear(formatted_words, MAX_PARA_LEN);
    }

    printf("\nta daa!\n");

    return 0;
}