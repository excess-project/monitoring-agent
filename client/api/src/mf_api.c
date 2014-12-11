#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <excess_main.h>    // createLogFile, prepare
#include <publisher.h>
#include <thread_handler.h> // prepSend

#include "mf_debug.h"
#include "mf_api.h"
#include "mf_types.h"

#define START_MONITORING 1
#define STOP_MONITORING 0

static int api_is_initialized = 0;

static long double get_time_in_ns();
static long double send_trigger();

void
mf_api_initialize(const char* URL, char* db_key)
{
    if (URL == NULL || strlen(URL) == 0) {
        log_error("mf_api_initialize() - URL is not set: %s", URL);
        exit(EXIT_FAILURE);
    }
    if (db_key == NULL || strlen(db_key) == 0) {
        log_error("mf_api_initialize() - DB key is not set: %s", db_key);
        exit(EXIT_FAILURE);
    }

    strcpy(execution_id, db_key);  // execution_id is defined in publisher.h
    strcpy(server_name, URL);  // server_name is defined in excess_main.h
    strcat(server_name, "/executions/");
    strcat(server_name, execution_id);

    createLogFile();
    api_is_initialized = 1;
}

long double
mf_api_start_profiling(const char* function_name)
{
    if (!api_is_initialized) {
        log_error("Library is not initialized: %s", "mf_api_start_profiling");
        exit(EXIT_FAILURE);
    }
    return send_trigger(function_name, START_MONITORING);
}

long double
mf_api_stop_profiling(const char* function_name)
{
    if (!api_is_initialized) {
        log_error("Library is not initialized: %s", "mf_api_stop_profiling");
        exit(EXIT_FAILURE);
    }
    return send_trigger(function_name, STOP_MONITORING);
}

static long double
send_trigger(const char* function_name, int flag)
{
    long double timestamp;

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

    timestamp = get_time_in_ns(resMetric->timestamp);
    free(resMetric);

    return timestamp;
}

static long double
get_time_in_ns(struct timespec date)
{
    long double timestamp = date.tv_sec +
        (long double) (date.tv_nsec / 10e8);

    return timestamp;
}