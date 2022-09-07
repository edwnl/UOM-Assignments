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
double getScore(StringList words, int start, int end, int argc, char *argv[]) {
    int key_indexes[argc], last_key_index = 0;
    double rule1 = 0, rule2 = 0, rule3 = 0, rule4 = 0, rule5 = 0, rule6 = 0;

    // ---- 1. Add 15 / (start + 10) points. First word = 0; ----
    rule1 += 15.0 / (start + 10.0);

    for (int i = start; i < end; ++i) {
        printf("%s", words[i]); // TODO: Store original words
        int key_index = isQuery(words[i], argc, argv);

        // If the word is a search term
        if(key_index != -1) {
            // ---- 3. Add 1.0 points for every other?? repeated query term ----
            for (int j = 0; j < argc; j++) {
                if(key_indexes[j] == key_index && (rule3++)) break;
            }

            // ---- 2. Add len(query) / 2 points for each unique query term ----
            if(rule3 == 0) {
                rule2 += (double) strlen(toAlpha(words[i])) / 2;
                key_indexes[last_key_index++] = key_index;
            }
        }
    }

    // ---- 4. Add 0.6 points if the word before is punctuated. (First word = punctuated) ----
    if(start == 0 || getPunctIndex(words[start - 1])) rule4 += 0.6;

    // ---- 5. Add 0.3 points if the ending word is punctuated. ----
    if(getPunctIndex(words[end - 1])) rule5 += 0.3;

    // ---- 6. Subtract 0.1 point for each word over MIN_SNIPPET_LEN ----
    rule6 += max((end - start) - MIN_SNIPPET_LEN, 0) * -0.1;

    printf("\nRules and Scores: \n[1]: %lf, [2]: %lf, [3]: %lf, [4]: %lf, [5]: %lf, [6]: %lf\n", rule1, rule2, rule3, rule4, rule5, rule6);

    return rule1 + rule2 + rule3 + rule4 + rule5 + rule6;
}

char * formatParagraph(const StringList words, int start, int finish, int argc, char *argv[], int *matches) {
    size_t char_count = 0, len;
    int punct_i, bold, wrap;
    StringList *output;
    *matches = 0;

    for (int i = start; i < finish; ++i) {
        String *word = strdup(words[i]), suffix, word_out;
        bold = isQuery(*word, argc, argv) != -1;
        len = strlen(*word) + (i != 0 ? 1 : 0) + (bold ? 4 : 0);
        wrap = char_count + len > MAX_OUTPUT_LINE;
        char_count = wrap ? len - 1 : char_count + len;

        // Check for bolding words with punctuation
        if (bold && (*matches++) && (strcat(suffix, BOLD))) {
            if(punct_i = getPunctIndex(word), punct_i != 0) {
                removeChar(*word, punct_i); // Remove punct char
                strcat(suffix, &word[i][punct_i]); // Add punct to suffix
            }
        }

        snprintf(word_out, sizeof word_out, "%s%s%s%s%s",
                 (i != 0 ? (wrap ? "\n" : " ") : ""), (bold ? BOLD : ""), *word,
                 suffix,
                 (i == finish - 1 ? "\n\n" : ""));

        strcpy(*output[i], word_out);

//        printf("String: %s Length: %d Wrap: %d Chars: %d\n",
//               word_out, len, wrap, char_count);
    }
    return output;
}

int main(int argc, char *argv[]) {
    String word;
    StringList words, *stage2, stage3;

    int status = 0, word_count = 0, para_count = 0, matches;

    while (status != EOF) {
        clear(stage2, MAX_PARA_LEN);

        // Get all words in paragraph
        while (status != PARA_END && status != EOF) {
            status = get_word(word, MAX_WORD_LEN);
            strcpy(words[word_count++], word);
        }

        printf("======= Stage 1 [para %d; %d words]\n", ++para_count, word_count);

        stage2 = (StringList *) formatParagraph(words, 0, word_count, argc, argv, &matches);

        printf("\n======= Stage 2 [para %d; %d words; %d matches]\n", para_count, word_count, matches);

        for (int i = 0; i < word_count; ++i) printf("%s", *stage2[i]);

//        printf("%s", *stage2);
//
//        printf("======= Stage 3 [para %d; %d words]\n", para_count, word_count);
//
//        printf("Score: %lf\n\n", getScore(words, 0, MIN_SNIPPET_LEN + 1, argc, argv));
//
        status = (status == EOF ? EOF : 0);
        word_count = 0;
    }

    return 0;
}





