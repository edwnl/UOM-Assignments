#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

int getOrCreateIndex(DF_t *data, action_t actn, int freq) {
    int r, **arr = data->arr;
    for (r = 0; r < data->evnts; r++) {
        if (arr[r][0] == actn) {
            arr[r][AMT_COL] += freq;
            return r;
        }
    }

    // Set the first col to the action, and init the amount.
    arr[r][0] = (int) actn;
    arr[r][AMT_COL] = freq;
    data->evnts += 1;
    return r;
}

DF_t *getDFArr(log_t *log) {
    DF_t *df = init_DF();

    // Loop through each log
    for (int i = 0; i < log->ndtr; ++i) {
        trace_t *trace = &log->trcs[i];
        event_t *event = trace->head;
        int last_indx = -1, indx;

        // Build the 2D Array
        while (event != NULL) {
            indx = getOrCreateIndex(df, event->actn, trace->freq);
            if (last_indx != -1) df->arr[last_indx][DF_COL + indx] += trace->freq;
            last_indx = indx;
            event = event->next;
        }
    }

    sort_df(df);
    return df;
}

char * get_patn(int patn) {
#define NAME(ERR) case ERR: return #ERR;
    switch (patn) {
        NAME(SEQ)
        NAME(CON)
        NAME(CHC)
        default: return "NONE";
    }
#undef NAME
}

int abstract_df(DF_t **df_ptr, log_t *log, int *abs, int stg, int n_evnts) {
//    printf("Abstract DF. ABS: %d, STG: %d\n", *abs, stg);
    DF_t *df = *(df_ptr);
    double max_weight = 0, weight = 0;
    int actn_a = 0, actn_b = 0, removed = 0, max_patn = NONE, patn;
    int FIRST_STG1 = *abs == 256, FIRST_STG2 = stg == 1;

    // Find the maximum weight of all the events.
    for (int r = 0; r < df->evnts; r++) {
        for (int c = 0; c < df->evnts; c++) {
            patn = find_pattern(df, &weight, r, c, stg, n_evnts);
//            printf("[%d, %d] Pattern: %s Weight: %.0lf\n", actn(df, r), actn(df, c), get_patn(patn), weight);
            if (weight > max_weight && patn != NONE) {
                max_weight = weight;
                max_patn = patn;
                actn_a = df->arr[r][ACTN_COL];
                actn_b = df->arr[c][ACTN_COL];
            }
        }
    }
//    printf("Pattern: %s Weight: %.0lf\n", get_patn(max_patn), max_weight);

    // Terminate if there is no more patterns.
    if (max_patn == NONE) return NONE;

    if (FIRST_STG1) printf("==STAGE 1============================\n");
    else if (FIRST_STG2) printf("==STAGE 2============================\n");
    else printf("=====================================\n");

    // Print the array, and the maximum weight.
    printDFArr(df);
    printf("%d = %s(", *abs, get_patn(max_patn));
    print_actn(actn_a);
    printf(",");
    print_actn(actn_b);
    printf(")\n");

    // Abstract 2 events into one, by replaceing with an abstract actn.
    for (int i = 0; i < log->ndtr; ++i) {
        if(DEBUG) printf("Trace amt: %d\n", log->trcs[i].freq);
        if(DEBUG) print_trace(&log->trcs[i]);
        // Start from the event in a trace.
        event_t *event = log->trcs[i].head;
        // If there is an event
        while (event != NULL) {
            // If either event is matched
            if (event->actn == actn_a || event->actn == actn_b) {
                // Set the current event to the abstract pattern.
                event->actn = *abs;

                // If the next event is, overwrite it.
                event_t *next = event->next;
                while(next != NULL && (next->actn == actn_b || next->actn == actn_a)) {
                    next = next->next;
                    removed += log->trcs[i].freq;
                }
                event->next = next;
            }
            // Iterate the next event
            event = event->next;
        }
        if(DEBUG) print_trace(&log->trcs[i]);
        if(DEBUG) printf("\n");
    }

    df->evnts -= 1;
    *abs += 1;

    if (DEBUG) printLog(log);

    // Overwrite the previous counter with
    free_DF(*df_ptr);
    // Calculate the new DF array after abstracting events.
    *df_ptr = getDFArr(log);

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
    while ((action = getchar()) != NEW_LINE) {
        // If this is the last trace, pass EOF signal through trace's freq.
        if (action == EOF) {
            trace->freq = EOF;
            return trace;
        }
        // Insert any alphanumeric characters to the trace.
        if (isalpha((int) action)) {
            trace = append(trace, action);
            *total_events += 1;
        }
    }
    return trace;
}

int main(int argc, char *argv[]) {
    log_t *log = init_log();
    trace_t *trace;
    int n_traces = 0, max_freq = 0, n_events = 0, curr_freq, eof, abs = 256, stg = 0;

    // Obtain a new trace from stdin, and update 'eof' to its frequency.
    while ((eof = (trace = get_trace(&n_events))->freq)) {
        // If the trace is already recorded, increment the record's frequency,
        // and free the memory of the current one.
        // Otherwise, record the trace.
        if ((curr_freq = find_trace(log, trace))) {
            free_trace(trace);
            trace = NULL;
        }
        else insert_trace(log, trace);

        // Update max frequency and the amount of traces.
        max_freq = curr_freq > max_freq ? curr_freq : max_freq;
        n_traces += 1;

        // The curr_freq field is used to pass an EOF signal.
        if (eof == EOF) break;
    }

    DF_t *df = getDFArr(log);

    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", df->evnts);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", n_events);
    printf("Total number of traces: %d\n", n_traces);
    printf("Most frequent trace frequency: %d\n", max_freq);

    sort_log(log);
    // Print most frequent tracess
    for (int i = 0; i < log->ndtr; i++) {
        if (log->trcs[i].freq == max_freq) print_trace(&log->trcs[i]);
    }

    // Print amount of each
    print_evnt_amt(df);

    while(abstract_df(&df, log, &abs, stg, n_events) != NONE);
    while(abstract_df(&df, log, &abs, ++stg, n_events) != NONE);

    printf("==THE END============================\n");

    free_trace(trace);
    trace = NULL;
    free_log(log);
    free_DF(df);
    return EXIT_SUCCESS;
}
