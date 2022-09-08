#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#ifndef ASSIGNMENT_1_HELPER_H
#define ASSIGNMENT_1_HELPER_H

#define MAX_WORD_LEN 23
#define MAX_PARA_LEN 10000

#define PARA_END 1
#define WORD_FND 2

#define TERM_PUNCT ".,;:!?"
#define NEEDS_DOTS ",;:"
#define PARA_SEPARATOR "\n\n"
#define BBOLD "**"
#define BBOLD_LEN (2 * strlen(BBOLD))
#define DDOTS "..."
#define TERM_CHARS "\n "
#define BYPASS_PUNCT "-'"

#define MAX_SNIPPET_LEN 30
#define MIN_SNIPPET_LEN 20
#define MAX_OUTPUT_LINE 72

#define MAX_TERMS 50
#define NO_MATCH (-1)

// Constant Types
typedef char String[MAX_WORD_LEN + 1];    // char[24]
typedef String Strings[MAX_PARA_LEN];  // char[10000][24]

// Helper functions
int max(int n1, int n2) { return n1 > n2 ? n1 : n2; }
int min(int n1, int n2) { return n1 < n2 ? n1 : n2; }
size_t getLastIndex(const char *str) { return strlen(str) - 1; }
void removeChar(char *str, int i) { strcpy(&str[i], "\0"); }
void clear(void *arr_ptr, const size_t size) { memset(arr_ptr, 0, size); }
int isPunctuated(const char c) { return strchr(TERM_PUNCT, c) != NULL; }

/*
 * Returns the index of the punctuation, or 0 if not found.
 */
int getPunctIndex(const char *c) {
    for (size_t i = getLastIndex(c); i > 0; i--)
        if (strchr(TERM_PUNCT, c[i]) != NULL) return i;
    return 0;
}

int alpha_len(const char *c) {
    int i = 0;
    for (; c[i]; i++)
        if (c[i] == EOF || !isalpha(c[i])) break;
    return i;
}

int need_dots(const char *c) {
    for (size_t i = getLastIndex(c); i > 0; i--) {
        if (strchr(NEEDS_DOTS, c[i]) != NULL) return 1;
        if (!isalpha(c[i]) && !strchr(BBOLD, c[i])) return 0;
        if (strchr(TERM_CHARS, c[i])) continue;
        break;
    }
    return 1;
}

char *getLower(const char *str) {
    char *dup = strdup(str);
    for (int i = 0; dup[i]; i++) dup[i] = tolower(dup[i]);

    return dup;
}

int isQuery(const char *str, const char *argv[]) {
    for (int j = 0; argv[j]; ++j)
        if (strstr(getLower(str), getLower(argv[j]))) return j;
    return -1;
}

int get_word(char *word, const int limit) {
    int chr = 0, len = 0;
    while (chr != EOF && !isalnum(chr = getchar()))
        ;
    if (chr == EOF) return EOF;

    while (limit > len && (word[len++] = chr) && (chr = getchar())) {
        if (isalnum(chr) || isPunctuated(chr))
            continue;  // Store alpha numeric & term punctuations.
        if (strchr(BYPASS_PUNCT, chr))
            chr = ' ';  // Replace bypass punctuation with space
        while (!strchr(TERM_CHARS, (chr)))
            chr = getchar();  // Ignore chars until space or new line.

        // Check for PARA_END or EOF
        if ((chr = getchar()) && chr == EOF || chr == '\n') {
            word[len] = '\0';
            return chr == EOF ? EOF : PARA_END;
        } else ungetc(chr, stdin);
        break;
    }

    word[len] = '\0';
    return WORD_FND;
}

#endif  // ASSIGNMENT_1_HELPER_H
