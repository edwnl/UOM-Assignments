//
// Created by edwin on 10/11/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"

trace_t* init_list() {
    trace_t *list;
    list = (trace_t*) malloc (sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    list->freq = 1;
    return list;
}

void swap(trace_t *a, trace_t *b) {
    trace_t temp = *a;
    *a = *b;
    *b = temp;
}

void print(trace_t *a) {
    event_t *evnt = a->head;
    while (evnt != NULL) {
        printf("%c, ", evnt->actn);
        evnt = evnt->next;
    }
}

int smallerTrace(trace_t *a, trace_t *b) {
    event_t *evnt_a = a->head, *evnt_b = b->head;
    // If the actions are the same
    while (evnt_a->actn == evnt_b->actn) {
        // Check the next node
        evnt_a = evnt_a->next; evnt_b = evnt_b->next;
        // If the end of the list is reached, the lists are identical.
        if(evnt_a == NULL && evnt_b == NULL) return 0;
    }

    return evnt_a->actn < evnt_b->actn;
}

void sort(log_t *log) {
    int i, j, min;
    // One by one move boundary of unsorted subarray
    for (i = 0; i < log->ndtr - 1; i++) {
        // Find the minimum element in unsorted array
        min = i;
        for (j = i + 1; j < log->ndtr; j++)
            if (smallerTrace(&log->trcs[j], &log->trcs[min]))
                min = j;

        // Swap the found minimum element
        // with the first element
        swap(&log->trcs[min], &log->trcs[i]);
    }
}

log_t* init_log() {
    log_t *log;

    // Allocate memory for the log itself
    log = (log_t*) malloc (sizeof(*log));
    assert(log!=NULL);

    // Allocate memory for the array of traces
    log->trcs = (trace_t*) malloc(DEF_TRACES * sizeof (trace_t));
    assert(log->trcs != NULL);

    log->cpct = DEF_TRACES;
    log->ndtr = 0;
    return log;
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

void printDFArr(DF_arr_t *DF_arr) {
    printf("   ");
    for (int i = 0; i < DF_arr->evnts; ++i) {
        printf("%c  ", DF_arr->arr[i][ACTN_COL]);
    }
    printf("\n");

    for (int i = 0; i < DF_arr->evnts; ++i) {
        printf("%c  ", DF_arr->arr[i][ACTN_COL]);
        for (int j = 0; j < DF_arr->evnts; ++j) {
            printf("%u  ", DF_arr->arr[i][j + DF_START_COL]);
        }
        printf("\n");
    }

    printf("-------------------------------------");
}
DF_arr_t * initDFArr() {
    DF_arr_t *DF_arr = malloc(sizeof (DF_arr_t));

    // Allocate memory for the log itself
    DF_arr->arr = malloc(DEF_UNIQ_EVNTS * sizeof(action_t *));
    for(int i = 0; i < DEF_UNIQ_EVNTS; i++){
        DF_arr->arr[i] = (action_t *) calloc(DEF_UNIQ_EVNTS, sizeof (action_t));
        assert(DF_arr->arr != NULL && DF_arr->arr[i] != NULL);
    }

    DF_arr->cpct = DEF_TRACES;
    DF_arr->evnts = 0;
    return DF_arr;
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
        log->trcs = realloc(log->trcs, log->cpct * 2 * sizeof (*log));
        assert(log->trcs != NULL);
    }
}
