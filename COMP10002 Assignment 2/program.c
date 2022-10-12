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

    sort(df);
    return df;
}

int abstract_actn(DF_t *df, log_t *log, int abs) {
    double max_weight = 0;
    int actn_a, actn_b, removed = 0;

    // Find the maximum weight of all the events.
    printf("%d", df->evnts);
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
    // Terminate if there is no more patterns.
    if (max_weight == 0) return ABS_FINISHED;

    // Print the array, and the maximum weight.
    printDFArr(df);
    printf("%d = SEQ(%c, %c)\n", abs, actn_a, actn_b);
    printLog(log);

    // Abstract the pattern and remove any events
    for (int i = 0; i < log->ndtr; ++i) {
        // Start from the event in a trace.
        event_t *event = log->trcs[i].head;

        // If there is an event
        while (event != NULL) {
            // If there is at least 2 events left, and the abstract pattern is matched.
            if (event->next != NULL && event->actn == actn_a && event->next->actn == actn_b) {
//                printf("Overwriting %c, %c with %d\n", event->actn, event->next->actn, abs);
                // Set the current event to the abstract pattern.
                event->actn = abs;
                // Skip the next event.
                event->next = event->next->next;
                // Increase the removed counter.
                removed += log->trcs[i].freq;
            }
            // Iterate the next event
            event = event->next;
        }
    }

    printLog(log);

    // Overwrite the previous counter with
    free_DF(df);
    // Calculate the new DF array after abstracting events.
    df = getDFArr(log);

    // Print the number of events removed
    printf("Number of events removed:a %d\n", removed);
    // Print how much of each event there is
    for (int i = 0; i < df->evnts; i++){
        printf(isAbs(df->arr[i][ACTN_COL]) ? "%d=%d\n" : "%c=%d\n",
               df->arr[i][ACTN_COL], df->arr[i][AMT_COL]);
    }
    printf("=====================================\n");


    return removed;
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
            append(trace, action);
            *total_events += 1;
        }
    }
    return trace;
}

int main(int argc, char *argv[]) {
    log_t *log = init_log();
    trace_t *trace;
    int amt_traces = 0, max_freq = 0, amt_events = 0, curr_freq, eof;
    action_t abs = 256;

    // Obtain a new trace from stdin, and update 'eof' to its frequency.
    while ((eof = (trace = get_trace(&amt_events))->freq)) {
        // If the trace is already recorded, increment the record's frequency,
        // and free the memory of the current one.
        // Otherwise, record the trace.
        if ((curr_freq = find_trace(log, trace))) free_trace(trace);
        else insert_trace(log, trace);

        // Update max frequency and the amount of traces.
        max_freq = curr_freq > max_freq ? curr_freq : max_freq;
        amt_traces += 1;

        // The curr_freq field is used to pass an EOF signal.
        if (eof == EOF) break;
    }

    DF_t *df = getDFArr(log);

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
//    abstract_actn(df,log,abs++);
    abstract_actn(df,log,abs++);
    abstract_actn(df,log,abs++);

//    while(abstract_actn(df,log,abs++) != ABS_FINISHED);
//
    printf("==STAGE 2============================\n");

    free_trace(trace);
    free_log(log);

    return EXIT_SUCCESS;
}