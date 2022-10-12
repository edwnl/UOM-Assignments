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
    DF_t *df = initDFArr();

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

    sort(df);
    return df;
}

int abstract_actn(DF_t *df, log_t *log, int abs) {
    double max_weight = 0;
    int actn_a, actn_b, removed = 0;

    printDFArr(df);

    for (int r = 0; r < df->evnts; r++) {
        for (int c = 0; c < df->evnts; c++) {
            double w = weight(df, r, c);

            if (w > max_weight) {
                max_weight = w;
                actn_a = df->arr[r][ACTN_COL];
                actn_b = df->arr[c][ACTN_COL];
            }
        }
    }

    if (max_weight == 0) return ABS_FINISHED;
    printf("%d = SEQ(%c, %c)\n", abs, actn_a, actn_b);

    for (int i = 0; i < log->ndtr; ++i) {
        event_t *event = log->trcs[i].head;

        while (event != NULL) {
            if (event->next != NULL && event->actn == actn_a && event->next->actn == actn_b) {
                event->actn = abs;
                event->next = event->next->next;
                removed += log->trcs[i].freq;
            }
            event = event->next;
        }
    }

    free(df);
    df = getDFArr(log);

    printf("Number of events removed: %d\n", removed);
    for (int i = 0; i < df->evnts; i++){
        printf(df->arr[i][0] > 255 ? "%d=%d\n" : "%c=%d\n",
               df->arr[i][0], df->arr[i][1]);
    }
    printf("=====================================\n");
    printDFArr(df);

    return (int) removed;
}

/**
 * Returns a linked list of events_t, aka a trace.
 */
trace_t *get_trace(int *total_events) {
    // Allocate memory for the trace.
    trace_t *trace = init_list();
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
            insert_at_foot(trace, action);
            *total_events += 1;
        }
    }
    return trace;
}

int main(int argc, char *argv[]) {
    DF_t *df;
    log_t *log = init_log();
    trace_t *trace;
    int amt_traces = 0, max_freq = 0, amt_events = 0, curr_freq, eof;
    action_t abs = 256;

    // Obtain a new trace from stdin, and update 'eof' to its frequency.
    while ((eof = (trace = get_trace(&amt_events))->freq)) {
        // If the trace is already recorded, increment the record's frequency,
        // and free the memory of the current one.
        // Otherwise, record the trace.
        if ((curr_freq = find_trace(log, trace))) free(trace);
        else insert_trace(log, trace);

        // Update max frequency and the amount of traces.
        max_freq = curr_freq > max_freq ? curr_freq : max_freq;
        amt_traces += 1;

        // The curr_freq field is used to pass an EOF signal.
        if (eof == EOF) break;
    }

    df = getDFArr(log);

    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", df->evnts);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", amt_events);
    printf("Total number of traces: %d\n", amt_traces);
    printf("Most frequent trace frequency: %d\n", max_freq);

    // Print most frequent traces
    for (int i = 0; i < log->ndtr; ++i) {
        if (log->trcs[i].freq == max_freq) {
            event_t *event = log->trcs[i].head;
            while (event != NULL) {
                printf("%c", event->actn);
                event = event->next;
            }
            printf("\n");
        }
    }
    // Print amount of each
    for (int i = 0; i < df->evnts; i++)
        printf("%c=%d\n", df->arr[i][0], df->arr[i][1]);

    printf("==STAGE 1============================\n");

    abstract_actn(df,log,abs);


    abs++;
//    printf("=====================================\n");
//    while(abstract_actn(df,log,abs) != ABS_FINISHED) {
//        abs++;
//        printf("=====================================");
//    }

    printf("==STAGE 2============================\n");

    return EXIT_SUCCESS;
}
