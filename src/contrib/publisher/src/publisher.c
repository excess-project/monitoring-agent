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
#include <stdlib.h>
#include <string.h>

#include "mf_debug.h"
#include "publisher.h"

static CURL *curl;
char execution_id[ID_SIZE] = { 0 };
struct curl_slist *headers = NULL;

static void init_curl()
{
	if (curl != NULL ) {
		return;
	}

    curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

    if (headers != NULL ) {
		return;
	}

	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");
}

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

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

static size_t get_stream_data(void *buffer, size_t size, size_t nmemb, void *stream) {
	size_t total = size * nmemb;
	memcpy(stream, buffer, total);

	return total;
}

static int check_URL(const char *URL)
{
    if (URL == NULL || *URL == '\0') {
        const char *error_msg = "URL not set.";
		log_error("publish(const char*, Message) %s", error_msg);
		return 0;
	}
	return 1;
}

static int check_message(char *message)
{
	if (message == NULL || *message == '\0') {
	    const char *error_msg = "message not set.";
		log_error("publish(const char*, Message) %s", error_msg);
		return 0;
	}
	return 1;
}

static int prepare_publish(const char *URL, char *message)
{
    init_curl();

	curl_easy_setopt(curl, CURLOPT_URL, URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long ) strlen(message));
	#ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

    return 1;
}

static int
prepare_query(const char* URL)
{
    init_curl();

    curl_easy_setopt(curl, CURLOPT_URL, URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    #ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

    return 1;
}

size_t curl_write( void *ptr, size_t size, size_t nmemb, struct string *stream)
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

int
query(const char* query, char* received_data)
{
    int result = SEND_SUCCESS;
    struct string response_message;

    if (!check_URL(query)) {
        return 0;
    }

    if (!prepare_query(query)) {
        return 0;
    }

    puts(query);

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
    //received_data = (char*) realloc (received_data, response_message.len);
    //strcpy(received_data, response_message.ptr);
    curl_easy_reset(curl);

    if(strstr(received_data, "Description") == NULL) {
        return 0;
    }
    return result;
}

int publish_json(const char *URL, char *message)
{
	int result = SEND_SUCCESS;

    if (!check_URL(URL) || !check_message(message)) {
        return 0;
    }

    if (!prepare_publish(URL, message)) {
        return 0;
    }

    #ifndef DEBUG
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_non_data);
    #else
    puts(message);
	#endif

	CURLcode response = curl_easy_perform(curl);
    if (response != CURLE_OK) {
		result = SEND_FAILED;
		const char *error_msg = curl_easy_strerror(response);
		log_error("publish(const char*, Message) %s", error_msg);
	}

	curl_easy_reset(curl);

    return result;
}

int publish(const char *URL, Message *messages)
{
    return SEND_SUCCESS;
}

char* get_execution_id(const char *URL, char *message)
{
    if (strlen(execution_id) > 0) {
	/*
        char* resp = malloc(100 * sizeof(char));
        memset(resp, 100, '\0');
        char query_url[300] = { '\0' };
        // e.g. http://localhost:3000/executions/add/:id
        sprintf(query_url, "%sadd/%s", URL, execution_id);

        if (publish_json(query_url, message)) {
            // Description message was sent.
            debug("%s is registered under http://localhost:3000/executions/", execution_id);
            return execution_id;
        }
	*/
	return execution_id;
    }

    if (!check_URL(URL) || !check_message(message)) {
        return '\0';
    }

    if (!prepare_publish(URL, message)) {
        return '\0';
    }

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_stream_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &execution_id);

	CURLcode response = curl_easy_perform(curl);
    if (response != CURLE_OK) {
		const char *error_msg = curl_easy_strerror(response);
		log_error("publish(const char*, Message) %s", error_msg);
	}

    debug("get_execution_id(const char*, char*) Execution_ID = <%s>", execution_id);

	curl_easy_reset(curl);

	return execution_id;
}

void shutdown_curl()
{
    if (curl == NULL ) {
		return;
	}

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
