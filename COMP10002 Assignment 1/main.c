#include <stdio.h>
#include <string.h>
#include "helper.h"

// Constant Values
#define PARA_END 1
#define WORD_FND 2

#define TERM_PUNCT ".,;:!?"
#define TERM_CHARS "\n "
#define BYPASS_PUNCT "-'"
#define NEEDS_DOTS ",;:"
#define BOLD "**"
#define DDOTS "..."

#define MAX_TERMS 50
#define MAX_WORD_LEN 23
#define MAX_PARA_LEN 10000
#define MAX_SNIPPET_LEN 30
#define MIN_SNIPPET_LEN 20
#define MAX_OUTPUT_LINE 72



/*
 * Start - inclusive
 */
double getScore(StringList words, int start, int end, char *argv[]) {
    int key_indexes[MAX_TERMS], last_key_index = 0, key_index;
    double rule1 = 0, rule2 = 0, rule3 = 0, rule4 = 0, rule5 = 0, rule6 = 0;

    // ---- 1. Add 15 / (start + 10) points. First word = 0; ----
    rule1 += 15.0 / (start + 10.0);

    for (int i = start; i < end; ++i) {
        if((key_index = isQuery(words[i], argv)) == -1) continue;

        // ---- 3. Add 1.0 points for every other repeated query term ----
        for (int j = 0; argv[j]; j++) if(key_indexes[j] == key_index && (rule3++)) break;

        // ---- 2. Add len(query) / 2 points for each unique query term ----
        if(rule3 == 0 && (rule2 += strlen(words[i]) / 2)) key_indexes[last_key_index++] = key_index;
    }

    // ---- 4. Add 0.6 points if the word before is punctuated. (First word = punctuated) ----
    if(start == 0 || getPunctIndex(words[start - 1])) rule4 += 0.6;

    // ---- 5. Add 0.3 points if the ending word is punctuated. ----
    if(getPunctIndex(words[end - 1])) rule5 += 0.3;

    // ---- 6. Subtract 0.1 point for each word over MIN_SNIPPET_LEN ----
    rule6 += max((end - start) - MIN_SNIPPET_LEN, 0) * -0.1;

    printf("[1]: %lf, [2]: %lf, [3]: %lf, [4]: %lf, [5]: %lf, [6]: %lf\n", rule1, rule2, rule3, rule4, rule5, rule6);

    return rule1 + rule2 + rule3 + rule4 + rule5 + rule6;
}

double getLowestScore(StringList words, int len, char *argv[], int *begin, int *finish) {
    if (len <= MIN_SNIPPET_LEN) return getScore(words, 0, len, argv);
    double score = 0, tmp_score;

    for (int start = 0; start < len; start++) {
        for (int end = min(len, start + MIN_SNIPPET_LEN); (end - start) <= MAX_SNIPPET_LEN; end++) {
            printf("\nStart: %d, End: %d\n", start, end);
            if((tmp_score = getScore(words, start, end, argv)) > score) {
                score = tmp_score;
                *begin = start;
                *finish = end;
                printf("Start: %d, End: %d, Score: %lf\n", *begin, *finish, score);
            }
        }
    }

    return score;
}

void printPara(const StringList formatWords, int start, int finish) {
    size_t char_count = 0, len, wrap;

    for (int i = start; i < finish; i++) {
        len = strlen(formatWords[i]) + (i != 0 ? 1 : 0);
        wrap = char_count + len > MAX_OUTPUT_LINE;
        char_count = wrap ? len - 1 : char_count + len;

        printf("%s%s%s", (i != 0 ? (wrap ? "\n" : " ") : ""), formatWords[i],
               (i == finish - 1 ? "\n\n" : ""));
    }
}

void formatPara(const StringList words, StringList output, char *argv[], int *matches) {
    int punct_i, bold, i = 0;
    for (*matches = 0; words[i][0]; i++) {
        String *word = strdup(words[i]), suffix = "", result = "";
        bold = isQuery(*word, argv) != -1;

        // Check for bolding words with punctuation
        if (bold) {
            *matches++;
            strcat(&suffix, BOLD);
            if(punct_i = getPunctIndex(word), punct_i != 0) {
                removeChar(*word, punct_i); // Remove punct char
                strcat(&suffix, &word[i][punct_i]); // Add punct to suffix
            }
        }

        snprintf(result, sizeof result,"%s%s%s", (bold ? BOLD : ""), *word, suffix);
        strcpy(output[i], result);
    }
}



int main(int argc, char *argv[]) {
    String word;
    StringList words, formatted_words;
    int status = 0, word_count = 0, para_count = 0, matches, start, end;

    while (status != EOF) {
        while (status != PARA_END && status != EOF) {
            status = get_word(word, MAX_WORD_LEN);
            strcpy(words[word_count++], word);
        }
        formatPara(words, formatted_words, argv, &matches);
        double score = getLowestScore(words, word_count, argv, &start, &end);

        printf("======= Stage 1 [para %d; %d words]\n", ++para_count, word_count);

        printf("\n======= Stage 2 [para %d; %d words; %d matches]\n", para_count, word_count, matches);
        printPara(formatted_words, 0, word_count);

        printf("======= Stage 3 [para %d; %d words; Score %.1lf]\n", para_count, word_count, score);
        printPara(formatted_words, start, end);

        status = (status == EOF ? EOF : 0);
        word_count = 0;
    }

    return 0;
}





