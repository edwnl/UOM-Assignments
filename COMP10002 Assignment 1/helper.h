#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifndef ASSIGNMENT_1_HELPER_H
#define ASSIGNMENT_1_HELPER_H

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

// Constant Types
typedef char String[MAX_WORD_LEN + 1];    // char[24]
typedef String StringList[MAX_PARA_LEN];  // char[10000][24]

// Helper functions

int max(int n1, int n2) { return n1 > n2 ? n1 : n2; }
int min(int n1, int n2) { return n1 < n2 ? n1 : n2; }

size_t getLastIndex(char *str) { return strlen(str) - 1; }

char getLast(char *str) { return str[getLastIndex(str)]; }

void removeChar(char *str, int i) { strcpy(&str[i], "\0"); }

void clear(void *arr_ptr, size_t size) { memset(arr_ptr, 0, size); }

int isPunctuated(char c) { return strchr(TERM_PUNCT, c) != NULL; }

/*
 * Returns the index of the punctuation, or 0 if not found.
 */
int getPunctIndex(char *c) {
    for (size_t i = getLastIndex(c); i > 0; i--)
        if (strchr(TERM_PUNCT, c[i]) != NULL) return i;
    return 0;
}

char *toAlpha(const char *str) {
    int *dup = strdup(str), *dup_ptr = dup, len = 0;

    while (*dup_ptr) {
        if (isalnum(*dup_ptr)) dup[len++] = *dup_ptr;
        dup_ptr++;
    }

    dup[len] = '\0';
    return dup;
}

char *getLower(const char *str) {
    char *dup = strdup(str);
    for (int i = 0; dup[i]; i++) dup[i] = tolower(dup[i]);

    return dup;
}

int isQuery(char *str, char *argv[]) {
    for (int j = 0; argv[j]; ++j)
        if (strstr(getLower(str), getLower(argv[j]))) return j;
    return -1;
}

int get_word(char *word, const int limit) {
    int chr = 0, len = 0;
    while (chr != EOF && !isalnum(chr = getchar()));
    if (chr == EOF) return EOF;

    while (limit > len && (word[len++] = chr) && (chr = getchar())) {
        if (isalnum(chr) || isPunctuated(chr))
            continue;  // Store alnum & term puncuations.
        if (strchr(BYPASS_PUNCT, chr))
            chr = ' ';  // Replace bypass puncuation with space
        while (!strchr(TERM_CHARS, (chr)))
            chr = getchar();  // Ignore chars until space or new line.

        // Check for PARA_END
        if ((chr = getchar()) == '\n') {
            word[len] = '\0';
            return PARA_END;
        } else ungetc(chr, stdin);
        break;
    }

    word[len] = '\0';
    return WORD_FND;
}

#endif  // ASSIGNMENT_1_HELPER_H
