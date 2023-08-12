/* Program to generate term-biased snippets for paragraphs of text.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2022, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* maximum number of characters per word */
#define MAX_WORD_LEN 23
/* maximum number of words per paragraph */
#define MAX_PARA_LEN 10000

/* return code from get_word if end of paragraph found */
#define PARA_END 1
/* return code from get_word if a word successfully found */
#define WORD_FND 2

/* terminating punctuation that may follow a word */
#define TERM_PUNCT ".,;:!?"
/* terminating punctuation that needs dots added */
#define NEEDS_DOTS ",;:"
/* the string the separates paragraphs */
#define PARA_SEPARATOR "\n\n"
/* insertion characters to indicate "bold" */
#define BBOLD "**"
/* length of those characters when wrapped around a word */
#define BBOLD_LEN 4
/* insertion characters to indicate omitted text */
#define DDOTS "..."

/* maximum words in any output snippet */
#define MAX_SNIPPET_LEN 30
/* minimum words in any output snippet */
#define MIN_SNIPPET_LEN 20
/* maximum length of output lines */
#define MAX_OUTPUT_LINE 72

/* maximum terms allowed on command line */
#define MAX_TERMS 50
/* signal for no match between word and query term */
#define NO_MATCH (-1)

// Normal Words
typedef char word_t[MAX_WORD_LEN + 1]; // Stores normal words
typedef word_t para_t[MAX_PARA_LEN + 1]; // Stores normal paragraphs

// Formatted Words - Format: [BBOLD](word)[BBOLD][Punctuation][\0]
typedef char fword_t[MAX_WORD_LEN + BBOLD_LEN + 2]; // Stores formatted words.
typedef word_t fpara_t[MAX_PARA_LEN + 1]; // Stores formatted word paragraphs.

// Helper Function prototypes
extern char* strdup(const char*);
int max(int n1, int n2);
int min(int n1, int n2);
int getLastIndex(const char *str);
int isPunct(const char c);
int isWordPunct(const char *str);
int alNumLen(const char *str);
int need_dots(const char *str);
char *getLowerAlNum(const char *str);
int isQuery(const char *str, const char *argv[]);

// Function Prototypes
int get_word(para_t words, const int *word_count);
void printPara(const fpara_t f_words, int start, int finish, int stg_3);
void formatPara(const para_t words, fpara_t output, const char *argv[],
                int *matches);
void calcLowestScore(const para_t words, int para_len, const char *argv[],
                     int *start_out, int *end_out, double *score);
double getScore(const para_t words, int start, int end, const char *argv[]);

/* Main Driver function to read input and print output.*/
int main(int argc, const char *argv[]) {
    // Temporary variables for the program.
    int getword_code, word_count, start, end, para_count = 0, matches = 0;
    double score = 0;
    // Stores normal and formatted paragraphs.
    para_t paragraph;
    fpara_t formatted_paragraph;

    // Loops through every paragraph until EOF.
    while (1) {
        // Reset variables for next paragraph.
        score = 0, matches = 0, word_count = 0;
        memset(paragraph, 0, MAX_PARA_LEN);
        memset(formatted_paragraph, 0, MAX_PARA_LEN);

        // Word Loop that stores all words into 'paragraph' from input.
        while (1) {
            getword_code = get_word(paragraph, &word_count);
            if (getword_code != WORD_FND) break;  // Break if EOF or PARA_END
            else word_count++;  // Increment word count
        }

        // Formats paragraph for Stage 2.
        formatPara(paragraph, formatted_paragraph, argv, &matches);
        // Computes the lowest score, start & end indices for Stage 3.
        calcLowestScore(paragraph, word_count, argv, &start, &end, &score);

        // Prints out results for each stage.
        printf("\n======= Stage 1 [para %d; %d words]\n"
                ,++para_count, word_count);

        printf("\n======= Stage 2 [para %d; %d words; %d matches]\n"
                ,para_count,word_count, matches);
        printPara(formatted_paragraph, 0, word_count, 0);

        printf("\n======= Stage 3 [para %d; start %d; length %d; score %.2lf]\n"
                ,para_count, start, end - start, score);
        printPara(formatted_paragraph, start, end, 1);

        if (getword_code == EOF) break;  // Terminate loop if EOF is reached.
    }
    printf("\nta daa!\n");
    return 0;
}

/**
 * Reads and stores one word from stdin. One char
 * of sensible trailing punctuation is retained.
 * Words MUST NOT be longer then MAX_WORD_LEN chars in length.
 * @param words Array where the read word is stored.
 * @param word_count Int position of the next word in words.
 * MUST NOT be greater then MAX_PARA_LEN.
 * @return EOF (End of File), PARA_END (Paragraph Ended) or
 * WORD_FOUND (Word Found).
 */
int get_word(para_t words, const int *word_count) {
    word_t word; // Word being read.
    char c; // Char from stdin.
    int word_len = 0, sep_index = 0; // Word length and index of separator.

    // Handles non-alphanumeric chars before the word.
    while (!isalnum(c = getchar())) {
        // Checks for PARA_END and EOF.
        sep_index = (c == PARA_SEPARATOR[sep_index] ? sep_index + 1 : 0);
        if (c == EOF) return EOF;
        if (sep_index == strlen(PARA_SEPARATOR)) return PARA_END;
    }

    // Loops through alphanumeric chars during the word.
    while (MAX_WORD_LEN > word_len) {
        word[word_len++] = c; // Stores the character.
        c = getchar();  // Collect the next char.

        if (!isalnum(c)) {
            // Store one trailing puncutation if found.
            if (isPunct(c)) word[word_len++] = c;
            // Escape the loop as char is no longer alphanumeric.
            break;
        }
    }

    // Appending the word to the output words array.
    word[word_len] = '\0';
    strcpy(words[*word_count], word);
    return WORD_FND;
}

/**
 * Given a list of words, and a start and end index, returns
 * the score of the snippet. Rules of scoring are detailed
 * within the function using comments.
 * @param words List of un-formatted words.
 * @param start Start index.
 * @param end End Index.
 * @param argv Command line arguments.
 * @return The score for the snippet, as a double.
 */
double getScore(const para_t words, int start, int end, const char *argv[]) {
    int indices[MAX_TERMS], len_indices = 0, key_index; // Stores key indices.
    double score = 15.0 / (start + 10.0); // + (15 / start + 10) points.
    memset(indices, 0, MAX_TERMS * sizeof(int)) ; // Clears indices.

    for (int i = start; i < end; i++) {
        // Check if the word is a query term.
        key_index = isQuery((char*) words[i], argv);

        // Continue if the word is not a query term
        if (key_index == NO_MATCH) continue;

        // Check if the query term is UNIQUE or REPEATED.
        for (int j = 0; j < MAX_TERMS; j++) {
            // + (query length / 2) points for every UNIQUE query term.
            if (!indices[j]) {
                score += (double)alNumLen(words[i]) / 2;
                indices[len_indices++] = key_index;
                break;
            }
            // + 1.0 points for every REPEATED query term.
            if (indices[j] == key_index && (score++)) break;
        }
    }

    // + 0.6 points if the word before is punctuated.
    if (start == 0 || isWordPunct((char*) words[start - 1])) score += 0.6;
    // + 0.3 points if the ending word is punctuated.
    if (isWordPunct((char*) words[end - 1])) score += 0.3;
    // - 0.1 point for each word over MIN_SNIPPET_LEN
    score += max((end - start) - MIN_SNIPPET_LEN, 0) * -0.1;

    return score;
}

/**
 * Finds the highest scoring snippet within a paragraph, by
 * looping all possible snippets using starting and ending indices,
 * where the (end - start) is between MIN and MAX_SNIPPET_LEN.
 * @param words List of un-formatted words.
 * @param para_len Length of words.
 * @param argv Command line arguments.
 * @param start_out Output start index.
 * @param end_out Output end index.
 * @param score Output Score.
 */
void calcLowestScore(const para_t words, int para_len, const char *argv[],
                     int *start_out, int *end_out, double *score) {
    int start = 0, end; // Start and End index;
    double cur_score; // Used to temporarily store a score.

    // start == 0 & end == para_len is for when para_len < MIN_SNIPPET_LEN.
    for (; start == 0 || start <= para_len - MIN_SNIPPET_LEN; start++) {
        end = min(para_len, start + MIN_SNIPPET_LEN);

        for (; end == para_len || end - start <= MAX_SNIPPET_LEN; end++) {
            // Calculates the score and updates vars if higher than prev score.
            cur_score = getScore(words, start, end, argv);
            if (cur_score > *score) {
                *score = cur_score;
                *start_out = start;
                *end_out = end;
            }
        }
    }
}

/**
 * Given a list of formatted words, prints them out from start to finish
 * without exceeding MAX_OUT_LINE chars per line, exclusive of line breaks.
 * @param f_words List of formatted words.
 * @param start Start Index
 * @param finish Finish Index
 * @param stg_3 Indicates Stage 3, as the ending of stage 3 words
 * requires DOTS.
 */
void printPara(const fpara_t f_words, int start, int finish, int stg_3) {
    int word_len, wrap, line_len = 0, dots, i;  // Temporary Variables.

    // Loops through all formatted words.
    for (i = start; i < finish; i++) {
        word_len = strlen(f_words[i]) + (i != start ? 1 : 0);  // Word Length
        wrap = line_len + word_len > MAX_OUTPUT_LINE;  // Is WRAPPING required?
        line_len = wrap ? word_len - 1 : line_len + word_len; // Line Length
        dots = stg_3 && i == finish - 1 && need_dots(f_words[i]); // Need Dots?

        // Prints out a word. Format: [SPACE or \n](WORD)[DOTS][\n]
        printf("%s%s%s%s", (i != start ? (wrap ? "\n" : " ") : ""), f_words[i],
               (dots ? DDOTS : ""), (i == finish - 1 ? "\n" : ""));
    }
}

/**
 * Given a list of un-formatted words, add BBOLD around
 * words which match query terms provided in argv.
 * @param words List of un-formatted words.
 * @param output List for outputting formatted words.
 * @param argv Command line arguments.
 * @param matches The number of query term matches.
 */
void formatPara(const para_t words, fpara_t output, const char *argv[],
                int *matches) {
    char suffix[BBOLD_LEN + 2]; // Suffix with format: [BBOLD][Punctuation][\0]
    // Temporary variables for function
    int punct_index, is_query;
    word_t word;
    fword_t result;

    for (int i = 0; words[i][0]; i++) {
        // Duplicate the word to be modified, and reset suffix result.
        strcpy(word, (char*) strdup(words[i]));
        strcpy(suffix, "");
        strcpy(result, "");

        // Update is_query by checking if the word matches a query term.
        is_query = isQuery((char*) &word, argv) != NO_MATCH;

        // If word is query, update matches variable.
        if (is_query && (*matches += 1)) {
            strcat(suffix, BBOLD);  // Append BBOLD to suffix

            // Handles bolding words with punctuation
            if (punct_index = isWordPunct((char*) &word), punct_index != 0) {
                // Remove punctuation char from word
                strcpy(&word[punct_index], "\0");
                // Append punctuation to suffix
                strcat(suffix, &words[i][punct_index]);
            }
        }

        // Store the formatted word. Format: [BBOLD](word)[BBOLD][PUNCTUATION]
        snprintf(result, sizeof result, "%s%s%s",
                 (is_query ? BBOLD : ""), (char*) &word, suffix);
        strcpy(output[i], result);
    }
}

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
 * The 0th character should never be a punctuation, otherwise its not a word!
 * @param str word_t to iterate.
 * @return Index of first TERM_PUNCT from the end. 0 if it doesn't exist.
 */
int isWordPunct(const char *str) {
    if (str[0] == '\0') return 0; // Handles NULL words.
    for (int i = getLastIndex(str); i > 0; i--)
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
        // TRUE, as last char is in NEEDS_DOTS or BBOLD or is alphanumeric.
        if (strchr(NEEDS_DOTS, str[i]) != NULL || isalnum(str[i]) ||
            strchr(BBOLD, str[i]) != NULL) return 1;
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
    char *dup = (char*) strdup(str);  // Duplicates the string.
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
        if (strcmp(getLowerAlNum((char*) str),
                   getLowerAlNum((char*) argv[j])) == 0) return j;
    return NO_MATCH;
}

// algorithms are fun