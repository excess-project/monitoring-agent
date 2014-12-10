#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <publisher.h>

#include "mf_api.h"
#include "mf_types.h"
#include "thread_handler.h" // prepSend
#include "excess_main.h"    // createLogFile, prepare

#define START_MONITORING 1
#define STOP_MONITORING 0

static void send_trigger(const char*, int);

void
mf_api_initialize(const char* conf_file, char* dbkey)
{
    if (dbkey != NULL) {
        strcpy(execution_id, dbkey);
    }

    createLogFile();
    prepare(conf_file);
}

void
mf_api_start_profiling(const char* function_name)
{
    send_trigger(function_name, START_MONITORING);
}

void
mf_api_stop_profiling(const char* function_name)
{
    send_trigger(function_name, STOP_MONITORING);
}

static void
send_trigger(const char* function_name, int flag)
{
    metric resMetric = malloc(sizeof(metric_t));
    resMetric->msg = malloc(100 * sizeof(char));

    int clk_id = CLOCK_REALTIME;
    clock_gettime(clk_id, &resMetric->timestamp);

    char *json = malloc(1024 * sizeof(char));
    char *status = malloc(256 * sizeof(char));

    strcpy(json, ",\"type\":\"user_function\"");
    sprintf(status, ",\"name\":\"%s\",\"status\":%d", function_name, flag);
    strcat(json, status);
    strcpy(resMetric->msg, json);
    free(json);

    prepSend(resMetric);
}