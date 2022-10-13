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

  Signed by: [Enter your full name and student number here before submission]
  Dated:     [Enter the date that you "signed" the declaration]
*/
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_TRACES 50
#define DEF_EVNTS 50
#define ACTN_COL 0
#define AMT_COL 1
#define DF_COL 2
#define NONE (-1)
#define SEQ 1
#define CON 2
#define CHC 3
#define EQUAL 1
#define SMALLER 2
#define GREATER 3
#define ABST(x) (x > 255)
#define MAX(a, b) (a > b ? a : b)
typedef int action_t;  // an action is identified by an integer

typedef struct event event_t;  // Forward Declaration
struct event {                 // ... an event is composed of ...
    action_t actn;               // ... an action that triggered it and ...
    event_t *next;               // ... a pointer to the next event in the trace
};

typedef struct {  // A linked list of events
    event_t *head;  // a pointer to the first event in this trace
    event_t *foot;  // a pointer to the last event in this trace
    int freq;       // the number of times this trace was observed
} trace_t;

typedef struct {  // An array of (distinct) traces sorted lexicographically
    trace_t *trcs;  // an array of traces
    int ndtr;       // the number of distinct traces in this log
    int cpct;  // the capacity of this event log as the number of distinct traces
    // it can hold
} log_t;

typedef struct {
    int **arr;
    int evnts;
    int cpct;
} DF_t;  // a directly follows relation over actions

// Helper Functions
void print_df_arr(DF_t *df);
void print_trace(trace_t *a);
void print_actn(int actn);
void print_evnt_amt(DF_t *df);
void sort_log(log_t *log);
DF_t *sort_df(DF_t *df);
DF_t *swap_df_indx(DF_t *df, int a, int b);
int find_trace(log_t *log, trace_t *target_trace);
int find_pattern(DF_t *df, double *weight, int r, int c, int stg, int n_evnts);
int actn(DF_t *df, int x);
double pd(DF_t *df, int x, int y);
int sup(DF_t *df, int r_indx, int c_indx);
int compareTrace(trace_t *a, trace_t *b);

// Data Structures
trace_t *init_trace();
trace_t *append(trace_t *list, action_t value);
log_t *init_log();
log_t *insert_trace(log_t *log, trace_t *trace);
DF_t *init_df();
void free_trace(trace_t *trace);
void free_log(log_t *log);
void free_df(DF_t *df);

// Main Functions
int getOrCreateIndex(DF_t *df, action_t actn, int freq);
DF_t *build_df(log_t *log);
char *get_patn(int patn);
int abstract_df(DF_t **df_ptr, log_t *log, int *abs, int stg, int n_evnts);
trace_t *get_trace(int *total_events);

int main(int argc, char *argv[]) {
    log_t *log = init_log();
    trace_t *trace;
    int n_trcs = 0, n_evnts = 0, max_freq = 0, freq, eof, abs = 256, stg = 0;

    // Collects traces from input, and creates a log.
    // The freq field is used to pass an EOF signal.
    while ((eof = (trace = get_trace(&n_evnts))->freq)) {
        // If the trace is already in the log:
        // Increment the logged trace's freq, free current trace.
        if ((freq = find_trace(log, trace))) free_trace(trace);
            // Otherwise, log the trace.
        else
            insert_trace(log, trace);
        trace = NULL;  // Trace pointer should no longer be used.

        // Update max frequency and the amount of traces.
        max_freq = freq > max_freq ? freq : max_freq;
        n_trcs++;

        if (eof == EOF) break;  // Escape the loop when EOF is read.
    }

    DF_t *df = build_df(log);
    sort_log(log);

    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", df->evnts);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", n_evnts);
    printf("Total number of traces: %d\n", n_trcs);
    printf("Most frequent trace frequency: %d\n", max_freq);

    // Print most frequent tracess
    for (int i = 0; i < log->ndtr; i++)
        if (log->trcs[i].freq == max_freq) print_trace(&log->trcs[i]);

    // Print amount of each
    print_evnt_amt(df);

    while (abstract_df(&df, log, &abs, stg, n_evnts) != NONE)
        ;
    while (abstract_df(&df, log, &abs, ++stg, n_evnts) != NONE)
        ;

    printf("==THE END============================\n");

    free_log(log);
    free_df(df);
    return EXIT_SUCCESS;
}

int getOrCreateIndex(DF_t *df, action_t actn, int freq) {
    int r, **arr = df->arr;
    for (r = 0; r < df->evnts; r++) {
        if (arr[r][0] == actn) {
            arr[r][AMT_COL] += freq;
            return r;
        }
    }

    // Set the first col to the action, and init the amount.
    arr[r][0] = (int)actn;
    arr[r][AMT_COL] = freq;
    df->evnts += 1;
    return r;
}

DF_t *build_df(log_t *log) {
    DF_t *df = init_df();

    // Loop through each log
    for (int i = 0; i < log->ndtr; ++i) {
        trace_t *trace = &log->trcs[i];
        event_t *event = trace->head;
        int last_indx = -1, indx;

        // Build the 2D Array
        while (event != NULL) {
            indx = getOrCreateIndex(df, event->actn, trace->freq);

            if (last_indx != -1) {
                df->arr[last_indx][DF_COL + indx] += trace->freq;
            }
            last_indx = indx;
            event = event->next;
        }
    }

    sort_df(df);
    return df;
}

int find_pattern(DF_t *df, double *weight, int r, int c, int stg2,
                 int n_evnts) {
    int sup_rc = sup(df, r, c), sup_cr = sup(df, c, r), patn = NONE;
    if (r == c) return patn;  // No pattern between equal events
    double pd_rc = pd(df, r, c);
    *weight = 0;

    if ((sup_rc > sup_cr) && pd_rc > 70) patn = SEQ;
    if (!stg2 && (ABST(actn(df, r)) || ABST(actn(df, c)))) return NONE;
    if (stg2 && sup_rc > 0 && sup_cr > 0 && pd_rc < 30) patn = CON;
    if (stg2 && MAX(sup_rc, sup_cr) <= (n_evnts / 100)) patn = CHC;

    *weight = fabs(50 - pd(df, r, c)) * MAX(sup_rc, sup_cr);
    if (patn == CHC) *weight = n_evnts * 100;
    if (patn == CON) *weight = *weight * 100;
    if (patn == SEQ && stg2 && !ABST(actn(df, r)) && !ABST(actn(df, c)))
        *weight = *weight * 100;
    return patn;
}

int abstract_df(DF_t **df_ptr, log_t *log, int *abs, int stg, int n_evnts) {
    DF_t *df = *(df_ptr);
    double max_weight = 0, weight = 0;
    int actn_a = 0, actn_b = 0, removed = 0, max_patn = NONE, patn;
    int FIRST_STG1 = *abs == 256, FIRST_STG2 = stg == 1;

    // Find the maximum weight of all the events.
    for (int r = 0; r < df->evnts; r++) {
        for (int c = 0; c < df->evnts; c++) {
            patn = find_pattern(df, &weight, r, c, stg, n_evnts);
            if (weight > max_weight && patn != NONE) {
                max_weight = weight;
                max_patn = patn;
                actn_a = df->arr[r][ACTN_COL];
                actn_b = df->arr[c][ACTN_COL];
            }
        }
    }

    // Terminate if there is no more patterns.
    if (max_patn == NONE) return NONE;

    if (FIRST_STG1)
        printf("==STAGE 1============================\n");
    else if (FIRST_STG2)
        printf("==STAGE 2============================\n");
    else
        printf("=====================================\n");

    // Print the array, and the maximum weight.
    print_df_arr(df);
    printf("%d = %s(", *abs, get_patn(max_patn));
    print_actn(actn_a);
    printf(",");
    print_actn(actn_b);
    printf(")\n");

    // Abstract 2 events into one, by replaceing with an abstract actn.
    for (int i = 0; i < log->ndtr; ++i) {
        // Start from the event in a trace.
        event_t *event = log->trcs[i].head;
        // If there is an event
        while (event != NULL) {
            // If either event is matched
            if (event->actn == actn_a || event->actn == actn_b) {
                // Set the current event to the abstract pattern.
                event->actn = *abs;
                // If the tmp_next event is, overwrite it.
                event_t *tmp_next = event->next, *tmp;
                while (tmp_next != NULL &&
                       (tmp_next->actn == actn_b||tmp_next->actn == actn_a)) {
                    tmp = tmp_next;
                    tmp_next = tmp_next->next;
                    removed += log->trcs[i].freq;
                    free(tmp);
                }
                event->next = tmp_next;
            }
            // Iterate the next event
            event = event->next;
        }
    }

    df->evnts -= 1;
    *abs += 1;

    // Overwrite the previous counter with
    free_df(*df_ptr);
    // Calculate the new DF array after abstracting events.
    *df_ptr = build_df(log);

    // Print the number of events removed
    printf("Number of events removed: %d\n", removed);

    // Print how much of each event there is
    print_evnt_amt(*df_ptr);
    return 1;
}

/**
 * Returns a linked list of events_t, aka a trace.
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

log_t *init_log() {
    log_t *log = malloc(sizeof *log);
    assert(log != NULL);

    // Allocate memory for the array of traces
    log->trcs = malloc(DEF_TRACES * sizeof *log->trcs);
    assert(log->trcs != NULL);

    log->cpct = DEF_TRACES;
    log->ndtr = 0;
    return log;
}

void free_log(log_t *log) {
    //    for (int i = 0; i < log->ndtr; i++) free_trace(&log->trcs[i]);
    free(log->trcs);
    free(log);
}

DF_t *init_df() {
    DF_t *df = malloc(sizeof *df);  // pointer itself
    assert(df != NULL);

    df->arr = malloc(DEF_EVNTS * sizeof *df->arr);  // rows
    assert(df->arr != NULL);

    for (int i = 0; i < DEF_EVNTS; i++) {
        df->arr[i] = calloc(DEF_EVNTS, sizeof *df->arr[i]);  // columns
        assert(df->arr[i] != NULL);
    }

    df->cpct = DEF_TRACES;
    df->evnts = 0;
    return df;
}

void free_df(DF_t *df) {
    for (int i = 0; i < df->cpct; i++) {
        free(df->arr[i]);  // free the columns
    }
    free(df->arr);  // free the rows
    free(df);       // free the pointer
}

void free_trace(trace_t *trace) {
    if (trace == NULL || trace->head == NULL) return;
    event_t *curr = trace->head, *prev;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(trace);
}

trace_t *init_trace() {
    trace_t *list = malloc(sizeof *list);
    assert(list != NULL);

    list->head = list->foot = NULL;
    list->freq = 1;
    return list;
}

trace_t *append(trace_t *list, action_t value) {
    event_t *event = malloc(sizeof *event);  // malloc for the event
    assert(list != NULL && event != NULL);

    // Build the event
    event->actn = value;
    event->next = NULL;

    // Insert the event
    if (list->foot == NULL) {
        list->head = list->foot = event;
    } else {
        list->foot->next = event;
        list->foot = event;
    }
    return list;
}

void print_trace(trace_t *a) {
    event_t *evnt = a->head;
    while (evnt != NULL) {
        print_actn(evnt->actn);
        evnt = evnt->next;
    }
    printf("\n");
}

DF_t *swap_df_indx(DF_t *df, int a, int b) {
    int *r_tmp, c_tmp;

    // Swapping Rows
    r_tmp = df->arr[a];
    df->arr[a] = df->arr[b];
    df->arr[b] = r_tmp;

    // Swapping Columns
    a += DF_COL;
    b += DF_COL;
    for (int row = 0; row < df->evnts; row++) {
        c_tmp = df->arr[row][a];
        df->arr[row][a] = df->arr[row][b];
        df->arr[row][b] = c_tmp;
    }

    return df;
}

int compareTrace(trace_t *a, trace_t *b) {
    event_t *evnt_a = a->head, *evnt_b = b->head;
    while (evnt_a->actn == evnt_b->actn) {
        evnt_a = evnt_a->next;
        evnt_b = evnt_b->next;
        if (evnt_a == NULL && evnt_b == NULL) return EQUAL;
        if (evnt_a == NULL || evnt_b == NULL)
            return (evnt_a == NULL) ? SMALLER : GREATER;
    }
    return (evnt_a->actn < evnt_b->actn) ? SMALLER : GREATER;
}

void sort_log(log_t *log) {
    for (int i = 0; i < log->ndtr - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < log->ndtr; j++)
            if (compareTrace(&log->trcs[j], &log->trcs[min_idx]) == SMALLER)
                min_idx = j;
        if (min_idx != i) {
            trace_t temp = log->trcs[min_idx];
            log->trcs[min_idx] = log->trcs[i];
            log->trcs[i] = temp;
        }
    }
}

DF_t *sort_df(DF_t *df) {
    for (int i = 0; i < df->evnts - 1; i++) {
        int min = i;
        for (int j = i + 1; j < df->evnts; j++)
            if (actn(df, j) < actn(df, min)) min = j;
        if (min != i) df = swap_df_indx(df, min, i);
    }
    return df;
}

void print_df_arr(DF_t *df) {
    int value;
    printf("%5s", " ");
    for (int i = 0; i < df->evnts; ++i) {
        value = df->arr[i][ACTN_COL];
        printf(ABST(value) ? "%5d" : "%5c", value);
    }
    printf("\n");

    for (int i = 0; i < df->evnts; ++i) {
        value = df->arr[i][ACTN_COL];
        printf(ABST(value) ? "%5d" : "%5c", value);
        for (int j = 0; j < df->evnts; ++j) {
            printf("%5u", df->arr[i][j + DF_COL]);
        }
        printf("\n");
    }
    printf("-------------------------------------\n");
}

log_t *insert_trace(log_t *log, trace_t *trace) {
    // Set frequency to one, since it's a new trace.
    trace->freq = 1;
    // Insert the trace into the array
    log->trcs[log->ndtr++] = *trace;
    // If the next trace will exhaust the space in the array
    if (log->ndtr > log->cpct) {
        // re-allocate double the memory to the array.
        log->trcs = realloc(log->trcs, log->cpct * 2 * sizeof(*log));
        assert(log->trcs != NULL);
    }
    return log;
}

int sup(DF_t *df, int r_indx, int c_indx) {
    return (int)df->arr[r_indx][c_indx + DF_COL];
}

double pd(DF_t *df, int x, int y) {
    if (MAX(sup(df, x, y), sup(df, y, x)) == 0) return -1;
    return 100 * abs(sup(df, x, y) - sup(df, y, x)) /
           MAX(sup(df, x, y), sup(df, y, x));
}

void print_evnt_amt(DF_t *df) {
    for (int i = 0; i < df->evnts; i++) {
        print_actn(actn(df, i));
        printf(" = %d\n", df->arr[i][AMT_COL]);
    }
}

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

int actn(DF_t *df, int x) { return df->arr[x][ACTN_COL]; }
void print_actn(int actn) { printf(ABST(actn) ? "%d" : "%c", actn); }

int find_trace(log_t *log, trace_t *target_trace) {
    for (int i = 0; i < log->ndtr; i++) {
        trace_t search = log->trcs[i];
        if (compareTrace(&search, target_trace) == EQUAL)
            return ++log->trcs[i].freq;
    }
    return 0;
}
// algorithms are fun