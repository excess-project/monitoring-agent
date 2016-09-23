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

#include <curl/curl.h>
#include <curl/multi.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <time.h>

#include "util.h"
#include "mf_debug.h"
#include "publisher.h"
#include "mf_parser.h"
#include "mf_types.h"

char execution_id[ID_SIZE] = { 0 };
struct curl_slist *headers = NULL;

/* Initialize libcurl; set headers format */
static void
init_curl()
{
    if (headers != NULL ) {
        return;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
}

/* Callback function for writing with libcurl */
#ifndef DEBUG
static size_t write_non_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}
#endif

struct string {
    char *ptr;
    size_t len;
};

/* malloc a string and initialize it */
void
init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

/* Callback function to get stream data during writing */
static size_t
get_stream_data(void *buffer, size_t size, size_t nmemb, void *stream) {
	size_t total = size * nmemb;
	memcpy(stream, buffer, total);

	return total;
}

/* Check if the url is set */
static int
check_URL(const char *URL)
{
    if (URL == NULL || *URL == '\0') {
        const char *error_msg = "URL not set.";
        log_error("publish(const char*, Message) %s", error_msg);
        return 0;
    }
    return 1;
}

/* check if the message is set */
static int
check_message(char *message)
{
    if (message == NULL || *message == '\0') {
        const char *error_msg = "message not set.";
        log_error("publish(const char*, Message) %s", error_msg);
        return 0;
    }
    return 1;
}

/* Prepare for using libcurl to write */
static CURL *
prepare_publish(const char *URL, char *message)
{
    init_curl();
    CURL *curl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long ) strlen(message));
    
    #ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

    return curl;
}

/* Prepare for using libcurl to read */
static CURL *
prepare_query(const char* URL)
{
    init_curl();
    CURL *curl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    #ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

    return curl;
}

/* Callback function to query*/
size_t
curl_write(void *ptr, size_t size, size_t nmemb, struct string *stream)
{
    stream->ptr = realloc(stream->ptr, size*nmemb+1);
    if (stream->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(stream->ptr, ptr, size*nmemb);
    stream->ptr[size*nmemb] = '\0';
    stream->len = size*nmemb;

    return fwrite(ptr, size, nmemb, stdout);
}

/* Data query using libcurl */
int
query(const char* query, char* received_data)
{
    int result = SEND_SUCCESS;
    struct string response_message;

    if (!check_URL(query)) {
        return 0;
    }

    CURL *curl = prepare_query(query);
    if (curl == NULL) {
        return 0;
    }

    init_string(&response_message);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_message);

    CURLcode response = curl_easy_perform(curl);
    if (response != CURLE_OK) {
        result = SEND_FAILED;
        const char *error_msg = curl_easy_strerror(response);
        log_error("query(const char*, char*) %s", error_msg);
    }
    if(response_message.len > 0){
    	strncpy(received_data, response_message.ptr, response_message.len);
    }
    curl_easy_cleanup(curl);

    if(strstr(received_data, "Description") == NULL) {
        return 0;
    }
    return result;
}

/* Metrics data publish using libcurl */
int
publish_json(const char *URL, char *message)
{
    int result = SEND_SUCCESS;

    if (!check_URL(URL) || !check_message(message)) {
        return 0;
    }
    CURL *curl = prepare_publish(URL, message);
    if (curl == NULL) {
        return 0;
    }

    #ifndef DEBUG
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_non_data);
    #endif

    CURLcode response = curl_easy_perform(curl);
    if (response != CURLE_OK) {
        result = SEND_FAILED;
        const char *error_msg = curl_easy_strerror(response);
        log_error("publish(const char*, Message) %s", error_msg);
    }

    curl_easy_cleanup(curl);
    return result;
}

/* Units publish using libcurl */
int
publish_unit(metric_units *units)
{
    int result = SEND_SUCCESS;
    int i = 0;
    char server[64];
    mfp_get_value("generic", "server", server);
    
    for (i=0; i < units->num_metrics; i++) {
        /* Remove white spaces in metric */
        char metric[128] = "\0";
        char *s;
        s = strchr(units->metric_name[i], ' ');
        if(s != NULL) {
            int len1 = (int)((intptr_t)s - (intptr_t)(units->metric_name[i]));
            strncpy(metric, units->metric_name[i], len1);
            strcat(metric, "%20");
            s++;
            strcat(metric, s);
        }
        else {
            strcpy(metric, units->metric_name[i]);
        }
        
        char *URL = malloc(128 * sizeof(char));
        sprintf(URL, "%s/v1/mf/units/%s", server, metric);
        char *msg = malloc(1000 * sizeof(char));
        sprintf(msg, "{\"name\":\"%s\",\"plugin\":\"%s\",\"unit\":\"%s\"}", 
            units->metric_name[i], units->plugin_name[i], units->unit[i]);
        if (!check_URL(URL) || !check_message(msg)) {
            return 0;
        }
        
        CURL *curl = prepare_publish(URL, msg);
        if (curl == NULL) {
            return 0;
        }
        
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");  /* !!! */

        CURLcode response = curl_easy_perform(curl);
        if (response != CURLE_OK) {
            result = SEND_FAILED;
            const char *error_msg = curl_easy_strerror(response);
            log_error("publish(const char*, Message) %s", error_msg);
        }
        curl_easy_cleanup(curl);
    }
    return result;
}

/* check if the units file of a plugin exists.
   if error, return -1; 
   if file NOT exists, create file and return 0;
   if file exists, return 1;
*/
int 
unit_file_check(const char *plugin_name) {
    char buf_1[200] = {'\0'};
    char buf_2[200] = {'\0'};
    char pwd[200] = {'\0'};
    char Filefolder[300] = {'\0'};
    char Filename[300] = {'\0'};

    /*get the host name, first 6 characters */
    char hostname[32] = {'\0'};
    char host[10] = {'\0'};
    getFQDN(hostname);
    strncpy(host, hostname, 6 * sizeof(char));

    readlink("/proc/self/exe", buf_1, 200);
    memcpy(buf_2, buf_1, strlen(buf_1) * sizeof(char));
    char *lastslash = strrchr(buf_2, '/');
    int ptr = lastslash - buf_2;
    memcpy(pwd, buf_2, ptr);
    sprintf(Filefolder, "%s/plugins/", pwd);
    sprintf(Filename, "%s/plugins/%s_units_%s", pwd, plugin_name, host);
    fprintf(stderr, "using logfile: %s\n", Filename);

    struct stat folder = { 0 };
    struct stat file = { 0 };
    if (stat(Filefolder, &folder) == -1) {
        fprintf(stderr, "Folder %s does not exist.\n", Filefolder);
        return -1;
    }
    if (stat(Filename, &file) == -1) {
        fprintf(stderr, "File %s does not exist.\n", Filename);
        FILE *fp = fopen(Filename, "w");
        if (fp == NULL) {
            fprintf(stderr, "Could not create file: %s\n", Filename);
            return -1;
        } else {
            time_t curTime;
            time(&curTime);
            struct tm *time_info = localtime(&curTime);
            char time_str[50];
            strftime(time_str, 50, "%F-%T", time_info);
            fprintf(fp, "mf_meminfo_units run at %s\n", time_str);
            return 0;
        }
    }
    else {
        fprintf(stderr, "file %s exists.\n", Filename);
        return 1;
    }
}

/* Register a new experiment and get the execution id */
char*
create_experiment_id(const char* URL, char* message)
{
    if (!check_URL(URL) || !check_message(message)) {
        return '\0';
    }
    CURL *curl = prepare_publish(URL, message);
    if (curl == NULL) {
        return '\0';
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_stream_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &execution_id);

    CURLcode response = curl_easy_perform(curl);
    if (response != CURLE_OK) {
        const char *error_msg = curl_easy_strerror(response);
        log_error("publish(const char*, Message) %s", error_msg);
    }

    debug("create_experiment_id(const char*, char*) Execution_ID = <%s>", execution_id);

    curl_easy_cleanup(curl);

    return execution_id;
}

/* Clean-up libcurl */
void
shutdown_curl()
{
    curl_global_cleanup();
}

/* Metrics data publish using ńon-blocking libcurl */
void* non_block_publish(const char *URL, char *message) {
    int still_running = 0;
    CURLM *m_curl=NULL;
    CURL *each=NULL;
    init_curl();
    if(m_curl == NULL) {
        m_curl = curl_multi_init();    
    }
    each = curl_easy_init();
    curl_easy_setopt(each, CURLOPT_URL, URL);
    curl_easy_setopt(each, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(each, CURLOPT_POSTFIELDS, message);
    curl_easy_setopt(each, CURLOPT_POSTFIELDSIZE, (long ) strlen(message));
    
    #ifdef DEBUG
    curl_easy_setopt(each, CURLOPT_VERBOSE, 1L);
    #endif
    #ifndef DEBUG
    curl_easy_setopt(each, CURLOPT_WRITEFUNCTION, write_non_data);
    #endif
    curl_multi_add_handle(m_curl, each);
    curl_multi_perform(m_curl, &still_running);
    return (void *)m_curl;
}

/* Free the finished multi-handles and handle in the multi-handle */
int curl_handle_clean(void *curl_ptr) {
    CURLM *m_curl = curl_ptr;
    int still_running = 0;
    curl_multi_perform(m_curl, &still_running);

    do {
        int numfds=0;
        int res = curl_multi_wait(m_curl, NULL, 0, MAX_WAIT_MSECS, &numfds);
        if(res != CURLM_OK) {
            fprintf(stderr, "error: curl_multi_wait() returned %d\n", res);
            return SEND_FAILED;
        }
        curl_multi_perform(m_curl, &still_running);
    } while(still_running);

    /* get the reponse message and remove each finished handle */
    CURL *eh=NULL;
    CURLMsg *msg=NULL;
    CURLcode return_code=0;
    int msgs_left=0;

    while ((msg = curl_multi_info_read(m_curl, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            eh = msg->easy_handle;
            return_code = msg->data.result;
            if(return_code != CURLE_OK) {
                fprintf(stderr, "CURL error code: %d\n", msg->data.result);
                continue;
            }
            curl_multi_remove_handle(m_curl, eh);
            curl_easy_cleanup(eh);
        }
        else {
            fprintf(stderr, "error: after curl_multi_info_read(), CURLMsg=%d\n", msg->msg);
        }
    }
    /* clean the multi curl structure */
    curl_multi_cleanup(m_curl);
    return SEND_SUCCESS;
}