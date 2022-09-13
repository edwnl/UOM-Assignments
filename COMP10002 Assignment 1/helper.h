#ifndef ASSIGNMENT_1_HELPER_H
#define ASSIGNMENT_1_HELPER_H

#define MAX_WORD_LEN 23     // Max chars per word.
#define MAX_PARA_LEN 10000  // Max words per paragraph.
#define MAX_SNIPPET_LEN 30  // Maximum length of snippets.
#define MIN_SNIPPET_LEN 20  // Minimum length of snippets.
#define MAX_OUTPUT_LINE 72  // Maximum characters of any output.
#define MAX_TERMS 50        // Maximum amount of query terms provided.

#define TRUE 1       // Used for infinite loop in main.
#define NO_MATCH -1  // Returns from is_query if the word isn't a query.
#define PARA_END 1   // Returns from get_word if it's the end of a paragraph.
#define WORD_FND 2   // Returns from get_word if a word is found.

#define TERM_PUNCT ".,;:!?"  // Terminating punctuation that may follow a word.
#define NEEDS_DOTS ",;:*"  // Terminating punctuation that requires dots added.
#define PARA_SEPARATOR "\n\n"  // String pattern that separates paragraphs.
#define DDOTS "..."  // Characters inserted to indicate omitted text.
#define BBOLD "**"   // Characters inserted before and after a bold word.
#define BBOLD_LEN 4 // Length of BBOLD when a word is surrounded.

// Normal Words
typedef char word_t[MAX_WORD_LEN + 1];
typedef word_t para_t[MAX_PARA_LEN + 1];

// Formatted Words - Format: [BBOLD](word)[BBOLD][Punctuation][\0]
typedef char fword_t[MAX_WORD_LEN + BBOLD_LEN + 2];
typedef word_t fpara_t[MAX_PARA_LEN + 1];

// Helper Function Prototypes
extern char* strdup(const char*);
int max(int n1, int n2);
int min(int n1, int n2);
int getLastIndex(const char *str);
int isPunct(const char c);
int getPunctIndex(const char *str);
int alNumLen(const char *str);
int need_dots(const char *str);
char *getLowerAlNum(const char *str);
int isQuery(const char *str, const char *argv[]);

/* --------------- Helper Functions! --------------- */
// Given two ints, returns the larger one.
int max(int n1, int n2) { return n1 > n2 ? n1 : n2; }
// Given two ints, returns the smaller one.
int min(int n1, int n2) { return n1 < n2 ? n1 : n2; }
// Returns the last index of a string.
int getLastIndex(const char *str) { return strlen(str) - 1; }
// Returns if a character is contained in TERM_PUNCT.
int isPunct(const char c) { return strchr(TERM_PUNCT, c) != NULL; }

/**
 * Returns the index of the first punctuation from the end of the word.
 * @param str word_t to iterate.
 * @return Index of first TERM_PUNCT from the end. 0 if it doesn't exist.
 */
int getPunctIndex(const char *str) {
    if (str[0] == '\0') return 0;
    for (int i = getLastIndex(str); i >= 0; i--)
        if (isPunct(str[i])) return i;
    return 0;
}

/**
 * Returns the alphanumeric length of a string, by iterating and stopping
 * at the first non-alphanumeric char.
 * @param str String to iterate.
 * @return String length until the first non-alphanumeric char.
 */
int alNumLen(const char *str) {
    int i = 0;
    for (; str[i]; i++)
        if (!isalnum(str[i])) return i;
    return i;
}

/**
 * Determines if an ending word requires dots.
 * Dots are required if the ending char is alphanumeric or in NEEDS_DOTS.
 * @param str String containing the ending word.
 * @return 1 if dots are required, 0 otherwise.
 */
int need_dots(const char *str) {
    // Iterates the word backwards to find the last char
    for (int i = getLastIndex(str); i >= 0; i--) {
        // Skip empty and new line chars.
        if (strchr("\n ", str[i])) continue;
        // TRUE, as last char is in NEEDS_DOTS or is alphanumeric.
        if (strchr(NEEDS_DOTS, str[i]) != NULL || isalnum(str[i])) return 1;
        break;
    }
    return 0;
}

/**
 * Returns a lower case copy of a string with punctuation removed.
 * All characters are ignored after the first non-alphanumeric char.
 * @param str Original string.
 * @return A processed copy of the string.
 */
char *getLowerAlNum(const char *str) {
    char *dup = (char *)strdup(str);  // Duplicates the string.
    // Loops through each char in the duped string.
    for (int i = 0; dup[i]; i++) {
        // Stop if a non-alphanumeric char is found.
        if (!isalnum(dup[i]) && (dup[i] = '\0')) break;
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
    for (int j = 1; argv[j]; ++j)  // 0th argument is the file path.
        if (strcmp(getLowerAlNum((char *)str),
                   getLowerAlNum((char *)argv[j])) == 0) return j;
    return NO_MATCH;
}

#endif //ASSIGNMENT_1_HELPER_H
