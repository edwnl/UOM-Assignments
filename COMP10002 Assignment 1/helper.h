#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifndef ASSIGNMENT_1_HELPER_H
#define ASSIGNMENT_1_HELPER_H

#define MAX_WORD_LEN 23
#define MAX_PARA_LEN 10000
#define MAX_SNIPPET_LEN 30
#define MIN_SNIPPET_LEN 20
#define MAX_OUTPUT_LINE 72
#define MAX_TERMS 50

#define TRUE 1
#define NO_MATCH -1
#define PARA_END 1
#define WORD_FND 2

#define TERM_PUNCT ".,;:!?"
#define NEEDS_DOTS ",;:*"
#define PARA_SEPARATOR "\n\n"
#define BBOLD "**"
#define DDOTS "..."

// Constant Types
typedef char String[MAX_WORD_LEN + 1];    // char[24]
typedef String Strings[MAX_PARA_LEN];  // char[10000][24]

// Helper functions
int max(int n1, int n2) { return n1 > n2 ? n1 : n2; }
int min(int n1, int n2) { return n1 < n2 ? n1 : n2; }
size_t getLastIndex(const char *str) { return strlen(str) - 1; }
void clear(void *arr, const size_t size) { memset(arr, 0, size); }
int isPunct(const char c) { return strchr(TERM_PUNCT, c) != NULL; }

/**
 * Returns the index of the first punctuation from
 * the end of the word.
 * @param str String to iterate.
 * @return Index of the first TERM_PUNCT from the end.
 * 0 if it doesn't exist.
 */
int getPunctIndex(const char *str) {
    for (size_t i = getLastIndex(str); i > 0; i--)
        if (strchr(TERM_PUNCT, str[i]) != NULL) return i;
    return 0;
}

/**
 * Returns the alphanumeric length of a String, by iterating
 * it, stopping at the first non-alphanumeric char.
 * @param str String to iterate.
 * @return String length until the first non-alphanumeric char.
 */
int alNumLen(const char *str) {
    int i = 0;
    for (; str[i]; i++) if (!isalnum(str[i])) return i;
    return i;
}

/**
 * Determines if an ending word requires dots.
 * Dots are required if the ending char is in NEEDS_DOTS,
 * or if it is alphanumeric.
 * @param c String containing the ending word.
 * @return 1 if dots are required, 0 otherwise.
 */
int need_dots(const char *c) {
    // Iterates the word backwards to find the last char..
    for (size_t i = getLastIndex(c); i > 0; i--) {
        // Skip empty and new line chars.
        if (strchr("\n ", c[i])) continue;
        // TRUE, as last char is in NEEDS_DOTS or is alphanumeric.
        if (strchr(NEEDS_DOTS, c[i]) != NULL || isalnum(c[i])) return 1;
        break;
    }
    return 0;
}

/**
 * Returns a lower case copy of a String with punctuation removed.
 * All characters are ignored after the first non-alphanumeric char.
 * @param str Original String.
 * @return A processed copy of the String.
 */
char *getLowerAlNum(const char *str) {
    char *dup = strdup(str); // Duplicates the String.

    // Loops through each char in the duped String.
    for (int i = 0; dup[i]; i++) {
        // Stop if a non-alphanumeric char is found.
        if(!isalnum(dup[i]) && (dup[i] = '\0')) break;

        // Otherwise, overwrite with a lowercase char.
        dup[i] = tolower(dup[i]);
    }

    return dup;
}

/**
 * Checks if a string matches a term provided as an argument.
 * Punctuation is skipped, and the comparison is case insensitive.
 * @param str String to check.
 * @param argv Program arguments.
 * @return The index of the argument, or -1 if it doesn't match.
 */
int isQuery(const char *str, const char *argv[]) {
    for (int j = 0; argv[j]; ++j)
        if (strcmp(getLowerAlNum(str), getLowerAlNum(argv[j])) == 0)
            return j;
    return NO_MATCH;
}

/**
 * Reads and stores one word from stdin. One char
 * of sensible trailing punctuation is retained.
 * @param words Array where the read word is stored.
 * @param word_count Int position of the next word in words.
 * @return EOF (End of File), PARA_END (Paragraph Ended)
 * or WORD_FOUND (Word Found).
 */
int get_word(Strings words, const int *word_count) {
    String word; // Word being read.
    char in_char; // Char from stdin.
    int word_len = 0, sep_index = 0; // Word length and index of separator.

    // Handles non-alphanumeric chars before the word.
    while (!isalnum(in_char = getchar())) {
        if (in_char == EOF) return EOF;

        // Checks for PARA_END by iterating PARA_SEPARATOR.
        sep_index = (in_char == PARA_SEPARATOR[sep_index] ? sep_index + 1 : 0);
        if(sep_index == strlen(PARA_SEPARATOR)) return PARA_END;
    }

    // Loops through alphanumeric chars during the word.
    while (MAX_WORD_LEN > word_len && (word[word_len++] = in_char)) {
        in_char = getchar(); // Collect the next character.

        if (isalnum(in_char)) continue;  // Stores alphanumeric chars.
        if (isPunct(in_char)) word[word_len++] = in_char; // Stores one trailing punctuation.
        break; // Escape if the char is no longer alphanumeric.
    }

    // Appending the word to the output words array.
    word[word_len] = '\0';
    strcpy(words[*word_count], word);

    return WORD_FND;
}

#endif  // ASSIGNMENT_1_HELPER_H
