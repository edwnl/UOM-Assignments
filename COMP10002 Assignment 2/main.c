#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define GOOD_LUCK   "GOOD LUCK CLASS!!!\n"
#define INIT_TRACES 50
#define NEW_LINE '\n'

typedef unsigned int action_t;  // an action is identified by an integer

// Event (a)
typedef struct event event_t;   // Forward Declaration
struct event {                  // ... an event is composed of ...
    action_t actn;              // ... an action that triggered it and ...
    event_t* next;              // ... a pointer to the next event in the trace
};

// Trace (a,b,c,d)
// This is a linked list of traces.
typedef struct {                // A linked list of events
    event_t* head;              // a pointer to the first event in this trace
    event_t* foot;              // a pointer to the last event in this trace
    int      freq;              // the number of times this trace was observed
} trace_t;

typedef struct {                // An array of (distinct) traces sorted lexicographically
    trace_t* trcs;              // an array of traces
    int      ndtr;              // the number of distinct traces in this log
    int      cpct;              // the capacity of this event log as the number of distinct traces it can hold
} log_t;

typedef action_t** DF_t;        // a directly follows relation over actions

trace_t* get_trace(int *total_events);
log_t* get_log();

trace_t* init_list() {
    trace_t *list;
    list = (trace_t*) malloc (sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    list->freq = 1;
    return list;
}

log_t* init_log() {
    log_t *log;
    trace_t *trace;

    // Allocate memory for the log itself
    log = (log_t*) malloc (sizeof(*log));
    assert(log!=NULL);

    // Allocate memory for the array of traces
    log->trcs = (trace_t*) malloc(INIT_TRACES * sizeof (*trace));
    assert(log->trcs != NULL);

    log->cpct = INIT_TRACES;
    log->ndtr = 0;
    return log;
}

int empty(trace_t *list) {
    assert(list!=NULL);
    return list->head==NULL;
}

void free_list(trace_t *list) {
    event_t *curr, *prev;
    assert(list!=NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}

trace_t *insert_at_head(trace_t *list, action_t value) {
    event_t *new;
    new = (event_t*)malloc(sizeof(*new));
    assert(list!=NULL && new!=NULL);
    new->actn = value;
    new->next = list->head;
    list->head = new;
    if (list->foot==NULL) {
        /* this is the first insertion into the list */
        list->foot = new;
    }
    return list;
}

trace_t *insert_at_foot(trace_t *list, action_t value) {
    event_t *event;
    event = (event_t*) malloc(sizeof(*event));
    assert(list!=NULL && event != NULL);
    event->actn = value;
    event->next = NULL;
    if (list->foot==NULL) {
        list->head = list->foot = event;
    } else {
        list->foot->next = event;
        list->foot = event;
    }
    return list;
}

action_t get_head(trace_t *list) {
    assert(list!=NULL && list->head!=NULL);
    return list->head->actn;
}

trace_t *get_tail(trace_t *list) {
    event_t *oldhead;
    assert(list!=NULL && list->head!=NULL);
    oldhead = list->head;
    list->head = list->head->next;
    if (list->head==NULL) {
        /* the only list node just got deleted */
        list->foot = NULL;
    }
    free(oldhead);
    return list;
}

int same_trace(event_t *a, event_t *b) {
    // If the actions are the same
    while(a->actn == b->actn) {
        // Check the next node
        a = a->next; b = b->next;
        // If the end of the list is reached, the list is identical.
        if(a == NULL && b == NULL) return 1;
    }
    return 0;
}

int find_trace(log_t *log, trace_t *target_trace) {
    for (int i = 0; i < log->ndtr; i++) {
        trace_t search = log->trcs[i];
        if (same_trace(search.head, target_trace->head))
            return ++log->trcs[i].freq;
    }
    return 0;
}

void insert_trace(log_t *log, trace_t *trace) {
    // Set frequency to one, since it's a new trace.
    trace->freq = 1;
    // Insert the trace into the array
    log->trcs[log->ndtr++] = *trace;
    // If the next trace will exhaust the space in the array
    if(log->ndtr > log->cpct) {
        // re-allocate double the memory to the array.
        log->trcs = realloc(log->trcs, log->cpct * 2 * sizeof (*trace));
        assert(log->trcs != NULL);
    }
}

int main(int argc, char *argv[]) {
    log_t *log = init_log();
    trace_t *trace;
    int amt_traces = 0, max_freq = 0, amt_events = 0, curr_freq, eof;

    // Obtain a new trace from stdin, and update 'eof' to its frequency.
    while((eof=(trace=get_trace(&amt_events))->freq)) {
        // If the trace is already recorded, increment the record's frequency,
        // and free the memory of the current one.
        // Otherwise, record the trace.
        if((curr_freq=find_trace(log, trace))) free(trace);
        else insert_trace(log, trace);

        // Update max frequency and the amount of traces.
        max_freq = curr_freq > max_freq ? curr_freq : max_freq;
        amt_traces += 1;

        // The curr_freq field is used to pass an EOF signal.
        if(eof == EOF) break;
    }

    //    for (int i = 0; i < log->ndtr; ++i) {
//        event_t *event = log->trcs[i].head;
//        while(event != NULL) {
//            printf("%c ", event->actn);
//            event = event->next;
//        }
//        printf("\n");
//    }

    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", 0);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", amt_events);
    printf("Total number of traces: %d\n", amt_traces);
    printf("Most frequent trace frequency: %d\n", max_freq);

    return EXIT_SUCCESS;
}

/**
 * Returns a linked list of events_t, aka a trace.
 */
trace_t* get_trace(int *total_events) {
    // Allocate memory for the trace.
    trace_t *trace = init_list();
    action_t action;

    // Loop over all chars until new line
    while ((action = getchar()) != NEW_LINE) {
        // If this is the last trace, pass EOF signal through trace's freq.
        if(action == EOF) {
            trace->freq = EOF;
            return trace;
        }
        // Insert any alphanumeric characters to the trace.
        if(isalpha((int) action)) {
            insert_at_foot(trace, action);
            *total_events += 1;
        }
    }
    return trace;
}
