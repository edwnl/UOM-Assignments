//
// Created by edwin on 10/11/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "utils.h"

trace_t* init_list() {
    trace_t *list = (trace_t*) malloc (sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    list->freq = 1;
    return list;
}

void print(trace_t *a) {
    event_t *evnt = a->head;
    while (evnt != NULL) {
        printf(isAbs(evnt->actn) ? "%d" : "%c", evnt->actn);
        evnt = evnt->next;
        if(evnt != NULL) printf(", ");
    }
    printf("\n");
}

void printdf(DF_t *df) {
    for (int r = 0; r < df->evnts; ++r) {
        for (int c = 0; c < df->evnts + DF_COL; ++c) {
            if(c == ACTN_COL && df->arr[r][c] < 255) {
                printf("[Row %d] %5c", r, df->arr[r][c]);
            } else if (c == AMT_COL) {
                printf("%5d events", df->arr[r][c]);
            } else {
                printf("%5d", df->arr[r][c]);
            }
        }
        printf("\n");
    }
}


void swap(DF_t *df, int a, int b) {
    int *r_tmp, c_tmp;

    r_tmp = df->arr[a];
    df->arr[a] = df->arr[b];
    df->arr[b] = r_tmp;

    a+= DF_COL; b += DF_COL;

    for (int row = 0; row < df->evnts; row++) {
        c_tmp = df->arr[row][a];
        df->arr[row][a] = df->arr[row][b];
        df->arr[row][b] = c_tmp;
    }
}

void sort(DF_t *df) {
    for (int i = 0; i < df->evnts - 1; i++) {
        int min = i;
        for (int j = i + 1; j < df->evnts; j++) {
            if(df->arr[j][ACTN_COL] < df->arr[min][ACTN_COL]) min = j;
        }
        if(min != i) swap(df, min, i);
    }
}

log_t* init_log() {
    log_t *log = (log_t*) malloc (sizeof(*log));
    assert(log!=NULL);

    // Allocate memory for the array of traces
    log->trcs = (trace_t*) malloc(DEF_TRACES * sizeof (trace_t));
    assert(log->trcs != NULL);

    log->cpct = DEF_TRACES;
    log->ndtr = 0;
    return log;
}

trace_t *append(trace_t *list, action_t value) {
    event_t *event = (event_t*) malloc(sizeof(*event));
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

        // Different length traces
        if((a == NULL && b != NULL) || (b == NULL && a != NULL)) return 0;
        // If the end of the list is reached, the list is identical.
        if(a == NULL && b == NULL) return 1;
    }
    return 0;
}

void printLog(log_t *log) {
    printf("Traces Count: %d, Capacity: %d\n", log->ndtr, log->cpct);
    for (int i = 0; i < log->ndtr; ++i) {
        print(&log->trcs[i]);
    }
}

int isAbs(int val) {
    return val > 255;
}

void printDFArr(DF_t *df) {
    printf("     ");
    for (int i = 0; i < df->evnts; ++i) {
        int value = df->arr[i][ACTN_COL];
        printf(value > 255 ? "%5d" :"%5c", df->arr[i][ACTN_COL]);
    }
    printf("\n");

    for (int i = 0; i < df->evnts; ++i) {
        int value = df->arr[i][ACTN_COL];
        printf(value > 255 ? "%5d" :"%5c", df->arr[i][ACTN_COL]);
        for (int j = 0; j < df->evnts; ++j) {
            printf("%5u", df->arr[i][j + DF_COL]);
        }
        printf("\n");
    }

    printf("-------------------------------------\n");
}
DF_t * initDFArr() {
    DF_t *DF_arr = malloc(sizeof (DF_t));

    // Allocate memory for the log itself
    DF_arr->arr = malloc(DEF_UNIQ_EVNTS * sizeof(int *));
    for(int i = 0; i < DEF_UNIQ_EVNTS; i++){
        DF_arr->arr[i] = (int *) calloc(DEF_UNIQ_EVNTS, sizeof (int));
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

int max(int a, int b) {return (a > b ? a : b);}

int sup(DF_t *df, int r_indx, int c_indx) {
    return (int) df->arr[r_indx][c_indx + DF_COL];
}

double pd(DF_t *df, int x, int y) {
    return (100 * abs(sup(df,x,y) - sup(df, y, x)) / max(sup(df, x, y), sup(df, y, x)));
}

double weight(DF_t *df, int r, int c) {
    if (r == c) return -1;
    if (sup(df, r, c) <= sup(df, c, r)) return -1;
    if (pd(df, r, c) <= 70) return -1;
    if (df->arr[r][ACTN_COL] > 255 || df->arr[c][ACTN_COL] > 255) return -1;

    return fabs(50 - pd(df, r, c)) * max(sup(df, r, c), sup(df, c, r));
}
