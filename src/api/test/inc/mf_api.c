/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>

#include <excess_main.h>    // createLogFile, prepare
#include <publisher.h>      // get_execution_id, query
#include <thread_handler.h> // prepSend
#include <util.h>           // getFQDN

#include "mf_debug.h"
#include "mf_api.h"
#include "mf_types.h"

#define START_MONITORING 1
#define STOP_MONITORING 0

static int api_is_initialized = 0;
static char statistics_url[256];
static char profiles_url[256];

static void check_api();
static long double send_trigger();
static void get_data_by_query();

static void
check_api()
{
    if (!api_is_initialized) {
        log_error("Library is not initialized. Please call %s first.", "mf_api_initialize");
        exit(EXIT_FAILURE);
    }
}

/*if the experiment is not registered by v1/mf/experiments, 
  create the experiment.
  return experiment_id */
char*
mf_api_create_experiment(const char* URL, char* wf_id, char* task_id)
{   
    if (URL == NULL || strlen(URL) == 0) {
        printf("mf_api_create_experiment() - URL is not set.\n");
        exit(EXIT_FAILURE);
    }
    if (wf_id == NULL || strlen(wf_id) == 0) {
        printf("mf_api_create_experiment() - wf_id is not set.\n");
        exit(EXIT_FAILURE);
    }
    if (task_id == NULL || strlen(task_id) == 0) {
        printf("mf_api_create_experiment() - task_id is not set.\n");
        exit(EXIT_FAILURE);
    }
    /* generating message */
    char msg[1000] = {'\0'};
    char time_stamp[64] = {'\0'};
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long double timestamp =  ts.tv_sec + (long double) (ts.tv_nsec / 1.0e9);
    convert_time_to_char(timestamp, time_stamp);

    if(hostname == NULL || strlen(hostname) == 0){
        hostname = malloc(256 * sizeof(char));
        hostname[0] = '\0';
        getFQDN(hostname);
        hostname[strlen(hostname) - 1] = '\0';
    }

    sprintf(msg,
        "{\"host\":\"%s\",\"@timestamp\":\"%s\",\"user\":\"%s\",\"application\":\"%s\"}"
        ,hostname, time_stamp, wf_id, task_id);

    /*generating experiment id*/
    char* path = malloc(256 * sizeof(char));
    sprintf(path,"%s/v1/mf/users/%s/create", URL, wf_id);
    char* exp_id = malloc(256 * sizeof(char));
    memset(exp_id, '\0', 256 * sizeof(char));
    strcpy(exp_id, create_experiment_id(path, msg));
    
    return exp_id;
}

/*mf api initialization with given URL, workflow, task, experiment_id
  prepare URLs for getting statistics data and profiles data */
void
mf_api_initialize(const char* URL, char* wf_id, char* exp_id, char* task_id)
{
    printf("mf_api_initialize...\n");
    if (URL == NULL || strlen(URL) == 0) {
        printf("mf_api_initialize() - URL is not set.\n");
        exit(EXIT_FAILURE);
    }
    if (wf_id == NULL || strlen(wf_id) == 0) {
        printf("mf_api_initialize() - workflow_id is not set.\n");
        exit(EXIT_FAILURE);
    }
    if (exp_id == NULL || strlen(exp_id) == 0) {
        printf("mf_api_initialize() - experiment_id is not set.\n");
        exit(EXIT_FAILURE);
    }
    if (task_id == NULL || strlen(task_id) == 0) {
        printf("mf_api_initialize() - task_id is not set.\n");
        exit(EXIT_FAILURE);
    }

    /* prepare hostname for prepSend and metrics interval statistics*/
    if(hostname == NULL || strlen(hostname) == 0) {
        hostname = malloc(256 * sizeof(char));
        hostname[0] = '\0';
        getFQDN(hostname);
        hostname[strlen(hostname) - 1] = '\0';
    }

    /* prepare task for prepSend*/
    if(task == NULL || strlen(task) == 0) {
        task = malloc(128 * sizeof(char));
        task[0] = '\0';
        strcpy(task, task_id);
    }
    
    /*initializing server name for sending metric data, if it is empty*/
    if ((server_name != NULL) && (server_name[0] == '\0')) {
        sprintf(server_name, "%s/v1/mf/metrics/%s/%s?task=%s", URL, wf_id, exp_id, task_id);
    }

    /*generating url for getting statistics data*/
    //URL should be "192.168.0.160:3030"
    memset(statistics_url, '\0', 256*sizeof(char));
    sprintf(statistics_url, "%s/v1/mf/statistics/%s/%s/%s", URL, wf_id, task_id, exp_id);

    /*generating url for getting profiles data*/
    memset(profiles_url, '\0', 256*sizeof(char));
    sprintf(profiles_url, "%s/v1/mf/profiles/%s/%s/%s", URL, wf_id, task_id, exp_id);

    createLogFile();
    api_is_initialized = 1;

}

/*get statistics data by metric*/
void 
mf_api_stats_data_by_metric(char *Metrics_name, char *res)
{
    char query_url[300] = {'\0'};

    // <stats_request_url> := http://localhost:3000/v1/mf/statistics/<workflow_id>/<task_id>/<experiment_id>
    // query_url := <stats_request_url>?metric=<metric>
    sprintf(query_url, "%s?metric=%s",
            statistics_url,
            Metrics_name);

    get_data_by_query(query_url, res);
}

/*get statistics data by metrics*/
void 
mf_api_stats_metrics(char **Metrics_name, int Metrics_num, char *res)
{
    int i;
    char query_url[300] = {'\0'};

    // <stats_request_url> := http://localhost:3000/v1/mf/statistics/<workflow_id>/<task_id>/<experiment_id>
    // query_url := <stats_request_url>?metric=<metric>
    sprintf(query_url, "%s?metric=%s",
            statistics_url,
            Metrics_name[0]);
    for (i = 1; i < Metrics_num; i++) {
        strcat(query_url, "&metric=");
        strcat(query_url, Metrics_name[i]);
    }

    get_data_by_query(query_url, res);
}

/*get statistics data of a metric during the given time interval*/
void 
mf_api_stats_data_by_interval(char *Metrics_name, long double start_time, long double stop_time, char *res)
{
    char start_timestamp[64] = {'\0'};
    char stop_timestamp[64]  = {'\0'};
    char query_url[300] = {'\0'};
    char host[10] = {'\0'};

    if(hostname != NULL && strlen(hostname) != 0) {
        strncpy(host, hostname, 6 * sizeof(char));
    }

    convert_time_to_char(start_time, start_timestamp);
    convert_time_to_char(stop_time, stop_timestamp);

    // <stats_request_url> := http://localhost:3000/v1/mf/statistics/<workflow_id>/<task_id>/<experiment_id>
    // query_url := <stats_request_url>?metric=<metric>&from=<start_timestamp>&to=<stop_timestamp>&host=<node01>
    sprintf(query_url, "%s?metric=%s&from=%s&to=%s&host=%s",
            statistics_url,
            Metrics_name,
            start_timestamp,
            stop_timestamp,
            host
           );

    get_data_by_query(query_url, res);
}

/*get statistics data of metrics during the given time interval*/
void 
mf_api_stats_metrics_by_interval(char **Metrics_name, int Metrics_num, long double start_time, long double stop_time, char *res)
{
    char start_timestamp[64] = {'\0'};
    char stop_timestamp[64]  = {'\0'};
    char query_url[300] = {'\0'};
    char host[10] = {'\0'};
    int i;

    if(hostname != NULL && strlen(hostname) != 0) {
        strncpy(host, hostname, 6 * sizeof(char));
    }

    convert_time_to_char(start_time, start_timestamp);
    convert_time_to_char(stop_time, stop_timestamp);

    // <stats_request_url> := http://localhost:3000/v1/mf/statistics/<workflow_id>/<task_id>/<experiment_id>
    // query_url := <stats_request_url>?metric=<metric>&from=<start_timestamp>&to=<stop_timestamp>&host=<node01>
    sprintf(query_url, "%s?metric=%s&from=%s&to=%s&host=%s",
            statistics_url,
            Metrics_name,
            start_timestamp,
            stop_timestamp,
            host
           );

    for (i = 1; i < Metrics_num; i++) {
        strcat(query_url, "&metric=");
        strcat(query_url, Metrics_name[i]);
    }
    get_data_by_query(query_url, res);
}

/* get all profiles data */
void 
mf_api_get_profiles_data(char *res)
{
    char query_url[300] = {'\0'};

    // e.g. http://localhost:3000/v1/mf/profiles/<workflow_id>/<task_id>/<experiment_id>
    sprintf(query_url, "%s",
            profiles_url);

    get_data_by_query(query_url, res);
}

static void
get_data_by_query(char* query_url, char *res)
{
    check_api();
    query(query_url, res);
}

/*return timestamps in long double data type*/
long double
mf_api_start_profiling(const char* function_name)
{
    check_api();
    return send_trigger(function_name, START_MONITORING);
}

/*return timestamps in long double data type*/
long double
mf_api_stop_profiling(const char* function_name)
{
    check_api();
    return send_trigger(function_name, STOP_MONITORING);
}

/*return timestamps in long double data type*/
static long double
send_trigger(const char* function_name, int flag)
{
    long double time_stamp;
    metric resMetric = malloc(sizeof(metric_t));
    resMetric->msg = malloc(100 * sizeof(char));

    int clk_id = CLOCK_REALTIME;
    clock_gettime(clk_id, &resMetric->timestamp);
    time_stamp = resMetric->timestamp.tv_sec + (long double) (resMetric->timestamp.tv_nsec/1.0e9);

    char *json = malloc(1024 * sizeof(char));
    char *status = malloc(256 * sizeof(char));

    strcpy(json, "\"type\":\"user_function\"");
    sprintf(status, ",\"name\":\"%s\",\"status\":%d", function_name, flag);
    strcat(json, status);
    strcpy(resMetric->msg, json);
    free(json);
    prepSend(resMetric);
    free(resMetric);
    return time_stamp;
}

/*send metrics data in json format*/
void
mf_api_send(const char* json)
{
    check_api();
    metric resMetric = malloc(sizeof(metric_t));
    resMetric->msg = malloc(100 * sizeof(char));

    int clk_id = CLOCK_REALTIME;
    clock_gettime(clk_id, &resMetric->timestamp);

    strcpy(resMetric->msg, json);
    prepSend(resMetric);
    free(resMetric);
}
