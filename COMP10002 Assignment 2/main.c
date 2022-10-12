#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

int getOrCreateIndex(DF_t *data, action_t actn, int freq) {
    int r;
    action_t **arr = data->arr;
    for (r=0; r < data->evnts; r++) {
//        printf("Checking r %d. Action At R: %u, Target: %u\n", r, arr[r][0], actn);
        if(arr[r][0] == actn) {
            arr[r][AMT_COL] += freq;
            return r;
        }
    }

    // Set the first col to the action, and init the amount.
    arr[r][0] = actn;
    arr[r][AMT_COL] = freq;
    data->evnts += 1;
    return r;
}

DF_t * getDFArr(log_t *log) {
    DF_t * data = initDFArr();
    sort(log);

    // Loop through each log
    for (int i = 0; i < log->ndtr; ++i) {
        trace_t *trace = &log->trcs[i];
        event_t *event = trace->head;
        int last_indx = -1, indx;

        // Loop through each event
        while(event != NULL) {
            // Get the index of the action
            indx = getOrCreateIndex(data, event->actn, trace->freq);
            if(last_indx != -1) data->arr[last_indx][DF_START_COL + indx] += trace->freq;
            last_indx = indx;

            event = event->next;
        }
    }

    return data;
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

    DF_t * df_arr = getDFArr(log);

    printf("==STAGE 0============================\n");
    printf("Number of distinct events: %d\n", df_arr->evnts);
    printf("Number of distinct traces: %d\n", log->ndtr);
    printf("Total number of events: %d\n", amt_events);
    printf("Total number of traces: %d\n", amt_traces);
    printf("Most frequent trace frequency: %d\n", max_freq);

    // Print most frequent traces
    for (int i = 0; i < log->ndtr; ++i) {
        if (log->trcs[i].freq == max_freq) {
            event_t *event = log->trcs[i].head;
            while(event != NULL) {
                printf("%c", event->actn);
                event = event->next;
            }
            printf("\n");
        }
    }
    // Print amount of each
    for (int i = 0; i < df_arr->evnts; i++)
        printf("%c=%d\n", df_arr->arr[i][0], df_arr->arr[i][1]);

    printf("==STAGE 1============================\n");
    printDFArr(df_arr);

    return EXIT_SUCCESS;
}
