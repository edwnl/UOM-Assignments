/* Program to discover a process model from an event log.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  August 2022, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.
*/
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEF_TRACES 15 // Default amount of Traces in a Log
#define DEF_EVNTS 15 // Default amount of events in the DF array.

#define NONE (-1) // Indicates something does not exist.
#define SEQ 1 // Signals a sequential pattern.
#define CON 2 // Signals a concurrent pattern.
#define CHC 3 // Signals a choice pattern.

#define EQUAL 1 // Signals two traces are equal.
#define SMALLER 2 // Signals a trace is smaller
#define GREATER 3 // Signals a trace is greater
#define ABST(x) ((x) > 255) // Returns if an event is abstract.
#define MAX(a, b) ((a) > (b) ? (a) : (b)) // Returns the larger number.

/* DF_t is a 2D array storing action, how many times the action is observed,
   and all directly follows relation over actions. Example:
+--------+--------+-------+--------+
| Action | Amount | DF(a) | (DF(b) |
+--------+--------+-------+--------+
|      a |      7 |     2 |      0 |
|      b |      7 |     3 |      4 |
+--------+--------+-------+--------+ */
#define ACTN_COL 0 // Column in DF array that stores the action.
#define AMT_COL 1 // Column in DF array that stores the amount of an action.
#define DF_COL 2 // Starting colun in DF array which stores the DF relations.

typedef struct {
    int **arr; // 2D Array
    int ndevnts; // number of distinct events in the 2D array
    int cpct; // capacity of the 2D array
} DF_t;

typedef int action_t;
typedef struct event event_t; // An event node used in trace_t
struct event {
    action_t actn;  // The Action of the event
    event_t *next; // Pointer to the next event in the trace
};

typedef struct {  // A linked list of events
    event_t *head;  // Pointer to the first event
    event_t *foot;  // Pointer to the last event
    int freq;       // Number of times this trace was observed
} trace_t;

typedef struct {  // An array of distinct traces sorted lexicographically
    trace_t *trcs; // Array of traces
    int ndtr; // the number of distinct traces in this log
    int cpct; // the capacity of this event log
} log_t;

trace_t *init_trace();
trace_t *append(trace_t *trace, int actn);
log_t *init_log();
log_t *insert_trace(log_t *log, trace_t *trace);
DF_t *init_df();
void free_trace(event_t *head);
void free_log(log_t *log);
void free_df(DF_t *df);
void print_df_arr(DF_t *df);
void print_trace(trace_t *a);
void print_actn(int actn);
void print_evnt_amt(DF_t *df);
void sort_log(log_t *log);
DF_t *sort_df(DF_t *df);
DF_t *swap_df_indx(DF_t *df, int a, int b);
int find_trace(log_t *log, trace_t *target_trace);
int find_pattern(DF_t *df, double *weight, int indx_a, int indx_b,
                 int stage, int n_evnts);
int get_actn(DF_t *df, int indx);
double pd(DF_t *df, int x, int y);
int sup(DF_t *df, int r_indx, int c_indx);
int compareTrace(trace_t *a, trace_t *b);
int getOrCreateIndex(DF_t *df, int actn, int freq);
DF_t *build_df(log_t *log);
char *get_patn(int patn);
int abstract_df(DF_t **df_ptr, log_t *log, int *abst, int stg, int n_evnts);
trace_t *get_trace(int *total_events);

int main(int argc, char *argv[]) {
    log_t *log = init_log(); // Initialise a log_t to store traces.
    trace_t *trace; // Used to temporarily hold traces.
    // No. traces, No. events, most frequent trace frequency.
    int n_trcs = 0, n_evnts = 0, max_trace_freq = 0, trace_freq;
    // Temp End of file variable, Abstract Event Name, Stage counter.
    int eof, abst = 256, stage = 0;

    // Collects traces from input, and creates a log.
    // get_trace()->freq used to pass an EOF signal.
    while ((eof = (trace = get_trace(&n_evnts))->freq)) {
        // Trace is already logged? Increment freq, and free current trace.
        // Trace is not logged? Add to logs.
        if ((trace_freq = find_trace(log, trace))) free_trace(trace->head);
        else insert_trace(log, trace);

        // Trace pointer must not be used again.
        free(trace);
        trace = NULL;

        // Update max frequency and the amount of traces.
        max_trace_freq = trace_freq > max_trace_freq
                ? trace_freq : max_trace_freq;
        n_trcs++;

        if (eof == EOF) break;  // Escape the loop when EOF is read.
    }

    // Build a directly follows array based on the collect log.
    DF_t *df = build_df(log);

    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", df->ndevnts);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", n_evnts);
    printf("Total number of traces: %d\n", n_trcs);
    printf("Most frequent trace frequency: %d\n", max_trace_freq);

    // Print all most frequent traces after sorting the log by ASCII code.
    sort_log(log);
    for (int i = 0; i < log->ndtr; i++)
        if (log->trcs[i].freq == max_trace_freq) print_trace(&log->trcs[i]);
    // Print the amount of each distinct event in df.
    print_evnt_amt(df);

    // Abstract the DF array, using only stage 1 rules.
    while (abstract_df(&df, log, &abst, stage, n_evnts) != NONE);
    // Abstract the DF array, using stage 1 & 2 rules.
    while (abstract_df(&df, log, &abst, ++stage, n_evnts) != NONE);

    printf("==THE END============================\n");
    // Free all malloc'd memory.
    free_log(log);
    free_df(df);
    return EXIT_SUCCESS;
}

/**
 * Get the index of an action in the DF array.
 * If the action does not exist, append it to the array.
 * If the action does exist, increment its amount column.
 * Array will be expanded if not enough memory is present.
 * @param df Pointer to DF array.
 * @param actn Action to query.
 * @param freq Frequency of the action.
 * @return Index of the action.
 */
int getOrCreateIndex(DF_t *df, action_t action, int freq) {
    // First, search for the event in DF.
    int index;
    for (index = 0; index < df->ndevnts; index++) {
        // If the action is found, increment data at AMT_COL.
        if (get_actn(df, index) == action) {
            df->arr[index][AMT_COL] += freq;
            return index;
        }
    }

    // The event does not exist. Check if DF is big enough to insert it.
    if (index + DF_COL >= df->cpct) {
        df->cpct = df->cpct * 2; // Double the capacity

        // Re-allocate pointer for rows
        df->arr = realloc(df->arr, df->cpct * sizeof *df->arr);
        assert(df->arr != NULL);

        // Re-allocate point for columns
        for (int j = 0; j < df->cpct; j++) {
            // Create a new larger space filled with 0s.
            int* new = calloc(df->cpct, sizeof *df->arr[j]);
            assert(new != NULL);

            // If an old pointer exists, copy the memory over and free it.
            if (j <= df->ndevnts) {
                memcpy(new, df->arr[j], df->cpct / 2);
                free(df->arr[j]);
            }

            // Update the DF array with the new pointer.
            df->arr[j] = new;
        }
    }

    // Insert it into the DF array.
    df->arr[index][0] = action;
    df->arr[index][AMT_COL] = freq;
    df->ndevnts += 1;
    return index;
}

/**
 * Constructs a DF structure given a log.
 * @param log Pointer to a log_t.
 * @return Pointer to a DF_t.
 */
DF_t *build_df(log_t *log) {
    DF_t *df = init_df(); // Initialise the DF array.

    // Constructs the DF array by looping through all traces.
    for (int i = 0; i < log->ndtr; ++i) {
        trace_t *trace = &log->trcs[i];
        event_t *event = trace->head;
        int last_indx = -1, indx;

        // Loop through each event on a trace.
        while (event != NULL) {
            // First get or create an index in the DF array.
            indx = getOrCreateIndex(df, event->actn, trace->freq);
            // Then, calculate the DF relationship based on the last event.
            if (last_indx != -1)
                df->arr[last_indx][DF_COL + indx] += trace->freq;
            // Update the last event and contiune the loop
            last_indx = indx;
            event = event->next;
        }
    }

    return sort_df(df); // Return the sorted DF array.
}

/**
 * Given two indices corrsponding to actions in the DF array,
 * calculate its pattern, and the weight of it.
 * @param df Pointer to the DF object.
 * @param weight Pointer to the weight.
 * @param indx_a Index of the first action.
 * @param indx_b Index of the second action.
 * @param stage Represent the stage. If stage != 0, stage 2 is implied.
 * @param n_evnts The total number of events in the log.
 * @return A pattern denoted by NONE, SEQ, CON or CHC.
 */
int find_pattern(DF_t *df, double *weight, int indx_a, int indx_b, int stage,
                 int n_evnts) {
    if (indx_a == indx_b) return NONE;  // No pattern between equal events

    // Calute sup(a, b), sup(b, a) and pd(a, b).
    int sup_ab = sup(df, indx_a, indx_b), sup_ba = sup(df, indx_b, indx_a);
    int patn = NONE;
    double pd_rc = pd(df, indx_a, indx_b);
    *weight = 0; // Reset the weight to 0.

    // Stage 1 will only find patterns between non-abstract events.
    if (!stage && (ABST(get_actn(df, indx_a))
    || ABST(get_actn(df, indx_b)))) return NONE;

    // Determine which pattern is matched based on given rules.
    if ((sup_ab > sup_ba) && pd_rc > 70) patn = SEQ;
    if (stage && sup_ab > 0 && sup_ba > 0 && pd_rc < 30) patn = CON;
    if (stage && MAX(sup_ab, sup_ba) <= (n_evnts / 100)) patn = CHC;

    // Calculate the weight of the pattern.
    *weight = fabs(50 - pd(df, indx_a, indx_b)) * MAX(sup_ab, sup_ba);

    // Overwrite the weight based on given rules.
    if (patn == CHC) *weight = n_evnts * 100;
    if (patn == CON || (patn == SEQ && stage &&
        !ABST(get_actn(df, indx_a)) && !ABST(get_actn(df, indx_b)))) {
        *weight = *weight * 100;
    }
    return patn;
}

/**
 * Abstracts a DF array.
 * @param df_ptr Pointer to DF Object.
 * @param log Pointer to log object.
 * @param abst Abstract event name.
 * @param stg Stage indicator.
 * @param n_evnts Total number of events.
 * @return 1 if success, and NONE if no more patterns exist.
 */
int abstract_df(DF_t **df_ptr, log_t *log, int *abst, int stg, int n_evnts) {
    DF_t *df = *(df_ptr);
    action_t a, b; // Temporary variables to store actions.
    double max_weight = 0, weight = 0;
    int removed = 0, max_patn = NONE, patn;

    // Find the pattern by looping each possible combination of indicies.
    for (int r = 0; r < df->ndevnts; r++) {
        for (int c = 0; c < df->ndevnts; c++) {
            patn = find_pattern(df, &weight, r, c, stg, n_evnts);
            // Update temprary variables if a better pattern is found.
            if (patn != NONE && weight > max_weight) {
                max_weight = weight;
                max_patn = patn;
                a = get_actn(df, r);
                b = get_actn(df, c);
            }
        }
    }

    if (max_patn == NONE) return NONE; // Return if no pattern found.

    // Print divisors. Add stage number if it's the first time running one.
    // abst == 256 if it's the first time running Stage 1.
    // stg == 1 if it's the first time running Stage 2.
    if (*abst == 256) printf("==STAGE 1============================\n");
    else if (stg == 1) printf("==STAGE 2============================\n");
    else printf("=====================================\n");

    // Print the array, and the pattern found, and it's events.
    print_df_arr(df);
    printf("%d = %s(", *abst, get_patn(max_patn));
    print_actn(a);
    printf(",");
    print_actn(b);
    printf(")\n");

    // Abstract 2 events into one, by replaceing with an abstract get_actn.
    for (int i = 0; i < log->ndtr; ++i) {
        // Start from the event in a trace.
        event_t *event = log->trcs[i].head;
        // If there is an event
        while (event != NULL) {
            // If either event is matched
            if (event->actn == a || event->actn == b) {
                // Set the current event to the abstract pattern.
                event->actn = *abst;
                // Overwrite any proceeding events that match the pattern.
                event_t *next = event->next, *tmp;
                while (next != NULL && (next->actn == a || next->actn == b)) {
                    tmp = next;
                    next = next->next;
                    removed += log->trcs[i].freq;
                    free(tmp);
                }
                event->next = next;
            }
            // Iterate the next event
            event = event->next;
        }
    }

    // Update the amount of events, and the abstract event name.
    df->ndevnts -= 1;
    *abst += 1;

    // Replace the DF object with a new one based on the current log.
    free_df(*df_ptr);
    *df_ptr = build_df(log);

    // Print the number of events removed, and amount of each event
    printf("Number of events removed: %d\n", removed);
    print_evnt_amt(*df_ptr);
    return 1;
}

/**
 * Returns a trace from STDIN.
 * @param total_events Total Events to update.
 */
trace_t *get_trace(int *total_events) {
    // Allocate memory for the trace.
    trace_t *trace = init_trace();
    action_t action;

    // Loop over all chars until new line
    while ((action = getchar()) != '\n') {
        // If this is the last trace, pass EOF signal through trace's freq.
        if (action == EOF) {
            trace->freq = EOF;
            return trace;
        }
        // Insert any alphanumeric characters to the trace.
        if (isalpha((int)action)) {
            trace = append(trace, action);
            *total_events += 1;
        }
    }
    return trace;
}

/**
 * Initialises a log object.
 * @return Pointer to the initliased object.
 */
log_t *init_log() {
    // Allocate memory for the pointer itself.
    log_t *log = malloc(sizeof *log);
    assert(log != NULL);

    // Allocate memory for the array of traces
    log->trcs = malloc(DEF_TRACES * sizeof *log->trcs);
    assert(log->trcs != NULL);

    // Set default values.
    log->cpct = DEF_TRACES;
    log->ndtr = 0;
    return log;
}

/**
 * Frees a log and all of its traces.
 */
void free_log(log_t *log) {
    for (int i = 0; i < log->ndtr; i++) free_trace(log->trcs[i].head);
    free(log->trcs);
    free(log);
}

/**
 * Initialises a DF object.
 * @return Pointer to the initliased object.
 */
DF_t *init_df() {
    // Allocate memory for the pointer itself.
    DF_t *df = malloc(sizeof *df);
    assert(df != NULL);

    // Allocate memory for the rows of the 2D array.
    df->arr = malloc(DEF_EVNTS * sizeof *df->arr);
    assert(df->arr != NULL);

    // Allocate memory for the columns of the 2D array.
    for (int i = 0; i < DEF_EVNTS; i++) {
        df->arr[i] = calloc(DEF_EVNTS, sizeof *df->arr[i]);  // columns
        assert(df->arr[i] != NULL);
    }

    // Set default values.
    df->cpct = DEF_TRACES;
    df->ndevnts = 0;
    return df;
}

/**
 * Frees a DF object.
 */
void free_df(DF_t *df) {
    for (int i = 0; i < df->cpct; i++) {
        free(df->arr[i]);  // free the columns
    }
    free(df->arr);  // free the rows
    free(df);       // free the pointer
}

/**
 * Frees a trace of events.
 */
void free_trace(event_t * head) {
    event_t *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

/**
 * Initialises a trace object.
 * @return Pointer to the initliased object.
 */
trace_t *init_trace() {
    // Allocate memory for the pointer itself.
    trace_t *list = malloc(sizeof *list);
    assert(list != NULL);

    // Set default values.
    list->head = list->foot = NULL;
    list->freq = 1;
    return list;
}

/**
 * Appends an action to a trace.
 * @param trace Trace to add tp.
 * @param actn Action to append.
 * @return Pointer to updated trace.
 */
trace_t *append(trace_t *trace, action_t actn) {
    // Allocate memory for the event.
    event_t *event = malloc(sizeof *event);
    assert(trace != NULL && event != NULL);

    // Build the event
    event->actn = actn;
    event->next = NULL;

    // Insert the event
    if (trace->foot == NULL) {
        trace->head = trace->foot = event;
    } else {
        trace->foot->next = event;
        trace->foot = event;
    }
    return trace;
}

/**
 * Prints a trace.
 */
void print_trace(trace_t *a) {
    event_t *evnt = a->head;
    while (evnt != NULL) {
        print_actn(evnt->actn);
        evnt = evnt->next;
    }
    printf("\n");
}

/**
 * Swaps the columns and rows of two indices in a DF array.
 * @param df DF object.
 * @param a First index.
 * @param b Second index.
 * @return Pointer to updated DF object.
 */
DF_t *swap_df_indx(DF_t *df, int a, int b) {
    int *r_tmp, c_tmp;

    // Swapping Rows
    r_tmp = df->arr[a];
    df->arr[a] = df->arr[b];
    df->arr[b] = r_tmp;

    // Swapping Columns
    a += DF_COL;
    b += DF_COL;
    for (int row = 0; row < df->ndevnts; row++) {
        c_tmp = df->arr[row][a];
        df->arr[row][a] = df->arr[row][b];
        df->arr[row][b] = c_tmp;
    }
    return df;
}

/**
 * Compares two traces and returns if the first one is
 * GREATER, EQUAL or SMALLER than the second one.
 * @param a Firsts trace.
 * @param b Second trace.
 * @return Result. (EQUAL, GREATER or SMALLER)
 */
int compareTrace(trace_t *a, trace_t *b) {
    event_t *evnt_a = a->head, *evnt_b = b->head;
    // Loop through events until the actions are no longer equal.
    while (evnt_a->actn == evnt_b->actn) {
        evnt_a = evnt_a->next;
        evnt_b = evnt_b->next;
        // If both lists end, they are equal.
        if (evnt_a == NULL && evnt_b == NULL) return EQUAL;
        // If one of the list ends, determine it's smaller.
        if (evnt_a == NULL || evnt_b == NULL)
            return (evnt_a == NULL) ? SMALLER : GREATER;
    }
    // Compare the differnt actions.
    return (evnt_a->actn < evnt_b->actn) ? SMALLER : GREATER;
}

/**
 * Sort a list of traces.
 * Comparisons made using the ASCII codes of the actions.
 * Shorter traces are deemed SMALLER.
 */
void sort_log(log_t *log) {
    // Selection Sort implementation simplicity.
    for (int i = 0; i < log->ndtr - 1; i++) {
        int min = i;
        for (int j = i + 1; j < log->ndtr; j++)
            if (compareTrace(&log->trcs[j], &log->trcs[min]) == SMALLER)
                min = j;
        if (min != i) {
            trace_t temp = log->trcs[min];
            log->trcs[min] = log->trcs[i];
            log->trcs[i] = temp;
        }
    }
}

/**
 * Sort a 2D DF array.
 * Comparisons made using the ASCII codes of the actions.
 */
DF_t *sort_df(DF_t *df) {
    // Selection sort implementation for simplicity.
    for (int i = 0; i < df->ndevnts - 1; i++) {
        int min = i;
        for (int j = i + 1; j < df->ndevnts; j++)
            if (get_actn(df, j) < get_actn(df, min)) min = j;
        if (min != i) df = swap_df_indx(df, min, i);
    }
    return df;
}

/**
 * Prints the directly follows relations from a DF 2D array.
 * @param df DF Object.
 */
void print_df_arr(DF_t *df) {
    int value;
    // Table Header
    printf("%5s", " ");
    for (int i = 0; i < df->ndevnts; ++i) {
        value = df->arr[i][ACTN_COL];
        printf(ABST(value) ? "%5d" : "%5c", value);
    }
    printf("\n");

    // Print each row
    for (int i = 0; i < df->ndevnts; ++i) {
        value = df->arr[i][ACTN_COL];
        printf(ABST(value) ? "%5d" : "%5c", value);
        for (int j = 0; j < df->ndevnts; ++j) {
            printf("%5u", df->arr[i][j + DF_COL]);
        }
        printf("\n");
    }
    printf("-------------------------------------\n");
}

/**
 * Inserts a new trace into a log.
 * @param log Log to insert into.
 * @param trace The trace to insert.
 * @return Updated log object.
 */
log_t *insert_trace(log_t *log, trace_t *trace) {
    // Expand log->trcs if the next trace will exceed the capacity.
    if (log->ndtr >= log->cpct) {
        log->cpct *= 2;
        log->trcs = realloc(log->trcs, log->cpct * sizeof *log->trcs);
        assert(log->trcs != NULL);
    }

    // Set frequency to one, since it's a new trace, and insert it.
    trace->freq = 1;
    log->trcs[log->ndtr++] = *trace;

    return log;
}

/**
 * Returns the support of two events, given their index.
 * @param df DF object.
 * @param r_indx Index of the first event.
 * @param c_indx Index of the second event.
 * @return The support recorded in DF object.
 */
int sup(DF_t *df, int r_indx, int c_indx) {
    return (int) df->arr[r_indx][c_indx + DF_COL];
}

/**
 * Return the percentage difference betwen two events, given their index.
 * @param df DF object.
 * @param x Index of the first event.
 * @param y Index of the second event.
 * @return The percentage different. -1 if both supports are 0. (Div/0)
 */
double pd(DF_t *df, int x, int y) {
    if (MAX(sup(df, x, y), sup(df, y, x)) == 0) return -1;
    return 100 * abs(sup(df, x, y) - sup(df, y, x)) /
           MAX(sup(df, x, y), sup(df, y, x));
}

/**
 * Given a DF object, print the amount of times each event appears.
 * @param df DF object.
 */
void print_evnt_amt(DF_t *df) {
    for (int i = 0; i < df->ndevnts; i++) {
        print_actn(get_actn(df, i));
        printf(" = %d\n", df->arr[i][AMT_COL]);
    }
}

/**
 * Given a pattern's status code, return the name of the pattern.
 * @param patn Status code of the pattern.
 * @return (SEQ, CON, CHC or NONE)
 */
char *get_patn(int patn) {
#define NAME(PATN) case PATN: return #PATN;
    switch (patn) {
        NAME(SEQ)
        NAME(CON)
        NAME(CHC)
        default:
            return "NONE";
    }
#undef NAME
}

/**
 * Returns the action correspoding to an index for a DF array.
 * @param df DF object
 * @param indx Index.
 */
action_t get_actn(DF_t *df, int indx) { return df->arr[indx][ACTN_COL]; }
// Prints an action. Abstract actions are printed as an int, otherwise char.
void print_actn(action_t actn) { printf(ABST(actn) ? "%d" : "%c", actn); }

/**
 * Finds a trace within the logs.
 * Increment the frequency of the log if found.
 * @param log Log object.
 * @param target_trace Trace to find.
 * @return 0 if not found, and it's updated frequency if found (> 0).
 */
int find_trace(log_t *log, trace_t *target_trace) {
    for (int i = 0; i < log->ndtr; i++) {
        trace_t search = log->trcs[i];
        if (compareTrace(&search, target_trace) == EQUAL)
            return ++log->trcs[i].freq;
    }
    return 0;
}
// algorithms are fun