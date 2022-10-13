//
// Created by edwin on 10/11/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "utils.h"


log_t* init_log() {
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
//    for (int i = 0; i < log->cpct; ++i) free_trace(&log->trcs[i]);
    free(log->trcs);
    free(log);
}

DF_t * init_DF() {
    DF_t *df = malloc(sizeof *df); // pointer itself
    assert(df != NULL);

    df->arr = malloc(DEF_EVNTS * sizeof *df->arr); // rows
    assert(df->arr != NULL);

    for (int i = 0; i < DEF_EVNTS; i++) {
        df->arr[i] = calloc(DEF_EVNTS, sizeof *df->arr[i]); // columns
        assert(df->arr[i] != NULL);
    }

    df->cpct = DEF_TRACES;
    df->evnts = 0;
    return df;
}

void free_DF(DF_t *df) {
    for (int i = 0; i < df->cpct; i++) {
        free(df->arr[i]);  // free the columns
    }
    free(df->arr);  // free the rows
    free(df);       // free the pointer
}

void free_trace(trace_t* trace) {
    if (trace == NULL) return;
    event_t *curr = trace->head, *prev;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(trace);
}

trace_t * init_trace() {
    trace_t *list = malloc(sizeof *list);
    assert(list!=NULL);

    list->head = list->foot = NULL;
    list->freq = 1;
    return list;
}

trace_t * append(trace_t *list, action_t value) {
    event_t *event = malloc(sizeof *event); // malloc for the event
    assert(list!=NULL && event != NULL);

    // Build the event
    event->actn = value;
    event->next = NULL;

    // Insert the event
    if (list->foot==NULL) {
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

DF_t * swap(DF_t *df, int a, int b) {
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

    return df;
}

int smallerTrace(trace_t *a, trace_t *b) {
    event_t *evnt_a = a->head, *evnt_b = b->head;
    // If the actions are the same
    while (evnt_a->actn == evnt_b->actn) {
        // Check the next node
        evnt_a = evnt_a->next; evnt_b = evnt_b->next;
        // Same length, both list reach the end (i.e. identical trace)
        if (evnt_a == NULL && evnt_b == NULL) return 0;
        // One of the events reached its end.
        // If it was event a, then return true as a is smaller.
        if (evnt_a == NULL || evnt_b == NULL) return evnt_a == NULL ? 1 : 0;
    }

    // First mismatch between traces
    return evnt_a->actn < evnt_b->actn;
}

void swap_trace(trace_t *a, trace_t *b) {
    trace_t temp = *a;
    *a = *b;
    *b = temp;
}

void sort_log(log_t *log) {
    for (int i = 0; i < log->ndtr-1; i++) {
        int min_idx = i;
        for (int j = i+1; j < log->ndtr; j++)
            if (smallerTrace(&log->trcs[j], &log->trcs[min_idx]))min_idx = j;
        if(min_idx != i) swap_trace(&log->trcs[min_idx], &log->trcs[i]);
    }
}

DF_t * sort_df(DF_t *df) {
    for (int i = 0; i < df->evnts - 1; i++) {
        int min = i;
        for (int j = i + 1; j < df->evnts; j++)
            if(actn(df, j) < actn(df, min))
                min = j;
        if(min != i) df = swap(df, min, i);
    }
    return df;
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
        print_trace(&log->trcs[i]);
    }
}

int isAbs(int val) {
    return val > 255;
}

void printDFArr(DF_t *df) {
    int value;
    printf("%5s", " ");
    for (int i = 0; i < df->evnts; ++i) {
        value = df->arr[i][ACTN_COL];
        printf(isAbs(value) ? "%5d" :"%5c", value);
    }
    printf("\n");

    for (int i = 0; i < df->evnts; ++i) {
        value = df->arr[i][ACTN_COL];
        printf(isAbs(value) ? "%5d" :"%5c", value);
        for (int j = 0; j < df->evnts; ++j) {
            printf("%5u", df->arr[i][j + DF_COL]);
        }
        printf("\n");
    }
    printf("-------------------------------------\n");
}

int find_trace(log_t *log, trace_t *target_trace) {
    for (int i = 0; i < log->ndtr; i++) {
        trace_t search = log->trcs[i];
        if (same_trace(search.head, target_trace->head))
            return ++log->trcs[i].freq;
    }
    return 0;
}



log_t * insert_trace(log_t *log, trace_t *trace) {
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
    return log;
}

int max(int a, int b) {return (a > b ? a : b);}

int sup(DF_t *df, int r_indx, int c_indx) {
    return (int) df->arr[r_indx][c_indx + DF_COL];
}

double pd(DF_t *df, int x, int y) {
    if (max(sup(df, x, y), sup(df, y, x)) == 0) return -1;
    return 100 * abs(sup(df,x,y) - sup(df, y, x)) / max(sup(df, x, y), sup(df, y, x));
}

int actn(DF_t *df, int x) {
    return df->arr[x][ACTN_COL];
}

void print_actn(int actn) {
    printf(isAbs(actn) ? "%d" : "%c", actn);
}

void print_evnt_amt(DF_t *df) {
    for (int i = 0; i < df->evnts; i++){
        print_actn(actn(df, i));
        printf(" = %d\n", df->arr[i][AMT_COL]);
    }
}

int find_pattern(DF_t *df, double *weight, int r, int c, int stg2, int n_evnts) {
    int sup_rc = sup(df, r, c), sup_cr = sup(df, c, r);
    int patn = NONE;
    *weight = 0;

    if(DEBUG) printf("Finding pattern for event %d, %d. \nsup_rc=%d sup_cr=%d\n", actn(df, r), actn(df, c), sup_rc, sup_cr);

    // No pattern between equal events
    if (r == c) {
        if(DEBUG) printf("[ERROR] No pattern between equal events.\n");
        return patn;
    }

    double pd_rc = pd(df, r, c);

    // SEQ: sup(r, c) > sup(c, r) && pd(r, c) > 70
    if((sup_rc > sup_cr) && pd_rc > 70) {
        if(DEBUG) printf("[SEQ] sup(r, c)=%d, sup(c, r)=%d, pd(r, c)=%lf\n", sup_rc, sup_cr, pd_rc);
        patn = SEQ;
    }

    if (!stg2) {
        // Stage 1 only identifies non-abstract patterns
        if (isAbs(actn(df, r)) || isAbs(actn(df, c))) {
            if(DEBUG) printf("[ERROR] No pattern involving at least one abstract event (Stage 1).\n");
            return NONE;
        }
    } else {
        if (sup_rc > 0 && sup_cr > 0 && pd_rc < 30) {
            if(DEBUG) printf("[CON] pd(r, c)=%lf\n", pd_rc);
            patn = CON;
        }

        if (max(sup_rc, sup_cr) <= (n_evnts / 100)) {
            if(DEBUG) printf("[CHC] max(sup_rc , sup_cr)=%d, (n_evnts / 100)=%d n_events=%d\n", max(sup_rc , sup_cr), (n_evnts / 100), n_evnts);
            patn = CHC;
        }
    }

    *weight = fabs(50 - pd(df, r, c)) * max(sup_rc, sup_cr);
    if (patn == CHC) *weight = n_evnts * 100;
    if (patn == CON) *weight = *weight * 100;
    if (patn == SEQ && stg2 && !isAbs(actn(df, r)) && !isAbs(actn(df, c)))
        *weight = *weight * 100;

    return patn;
}
