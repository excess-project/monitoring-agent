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

static void check_api();
static long double get_time_in_ns();
static long double send_trigger();
static char* get_data_by_query();

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
    check_api();
    return send_trigger(function_name, START_MONITORING);
}

long double
mf_api_stop_profiling(const char* function_name)
{
    check_api();
    return send_trigger(function_name, STOP_MONITORING);
}

static void check_api()
{
    if (!api_is_initialized) {
        log_error("Library is not initialized. Please call %s first.", "mf_api_initialize");
        exit(EXIT_FAILURE);
    }
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

char*
get_data_by_interval(long double start_time, long double stop_time)
{
    char query_url[300] = { '\0' };

    // <server_name> := http://localhost:3000/executions/
    // query_url := <server_name>/<db_key>/<start_time>/<stop_time>
    sprintf(query_url, "%s/%.9Lf/%.9Lf",
        server_name,
        start_time,
        stop_time
    );

    return get_data_by_query(query_url);
}

char*
get_data_by_metric_by_interval(
    const char* metric_name,
    long double start_time,
    long double stop_time)
{
    char query_url[300] = { '\0' };

    // <server_name> := http://localhost:3000/executions/
    // query_url := <server_name>/<db_key>/<metric_name>/<start_time>/<stop_time>
    sprintf(query_url, "%s/%s/%.9Lf/%.9Lf",
        server_name,
        metric_name,
        start_time,
        stop_time
    );

    return get_data_by_query(query_url);
}

static char*
get_data_by_query(char* query_url)
{
    check_api();

    char* response = malloc(10000 * sizeof(char));
    memset(response, 10000, '\0');

    query(query_url, response);

    return response;
}

void
mf_api_send(const char* json)
{
    metric resMetric = malloc(sizeof(metric_t));
    resMetric->msg = malloc(100 * sizeof(char));

    int clk_id = CLOCK_REALTIME;
    clock_gettime(clk_id, &resMetric->timestamp);
    strcpy(resMetric->msg, json);

    prepSend(resMetric);
}