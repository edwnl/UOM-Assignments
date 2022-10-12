//
// Created by edwin on 10/11/2022.
//
#ifndef COMP10002_ASSIGNMENT_2_UTILS_H
#define COMP10002_ASSIGNMENT_2_UTILS_H

#define DEF_TRACES 50
#define DEF_UNIQ_EVNTS 10
#define NEW_LINE '\n'
#define ACTN_COL 0
#define AMT_COL 1
#define DF_START_COL 2

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

typedef struct {
    action_t ** arr;
    int evnts;
    int cpct;
} DF_t; // a directly follows relation over actions

void printDFArr(DF_t *DF_arr);
trace_t* init_list();
void swap(trace_t *a, trace_t *b);
void print(trace_t *a);
int smallerTrace(trace_t *a, trace_t *b);
void sort(log_t *log);
log_t* init_log();
trace_t *insert_at_foot(trace_t *list, action_t value);
int same_trace(event_t *a, event_t *b);
DF_t * initDFArr();
int find_trace(log_t *log, trace_t *target_trace);
void insert_trace(log_t *log, trace_t *trace);

#endif //COMP10002_ASSIGNMENT_2_UTILS_H
