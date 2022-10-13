//
// Created by edwin on 10/11/2022.
//
#ifndef COMP10002_ASSIGNMENT_2_UTILS_H
#define COMP10002_ASSIGNMENT_2_UTILS_H

#define DEF_TRACES 50
#define DEF_EVNTS 50
#define NEW_LINE '\n'

#define ACTN_COL 0
#define AMT_COL 1
#define DF_COL 2

#define NONE (-1)

#define SEQ 1
#define CON 2
#define CHC 3


typedef int action_t;  // an action is identified by an integer

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
    int ** arr;
    int evnts;
    int cpct;
} DF_t; // a directly follows relation over actions

void printDFArr(DF_t *df);
DF_t * swap(DF_t *df, int a, int b);
void print(trace_t *a);
DF_t * sort(DF_t *df);
int isAbs(int val);
void printLog(log_t *log);
int same_trace(event_t *a, event_t *b);
int find_trace(log_t *log, trace_t *target_trace);
log_t * insert_trace(log_t *log, trace_t *trace);
int actn(DF_t *df, int x);
int find_pattern(DF_t *df, double *weight, int r, int c, int stg2, int n_evnts);


// Data Structures
trace_t* init_trace();
trace_t *append(trace_t *list, action_t value);
log_t* init_log();
DF_t * init_DF();
void free_trace(trace_t* trace);
void free_log(log_t *log);
void free_DF(DF_t *df);

#endif //COMP10002_ASSIGNMENT_2_UTILS_H
