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

static size_t
write_data(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
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

size_t curl_write( void *ptr, size_t size, size_t nmemb, void *stream)
{
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
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_message);

    CURLcode response = curl_easy_perform(curl);
    if (response != CURLE_OK) {
        result = SEND_FAILED;
        const char *error_msg = curl_easy_strerror(response);
        log_error("query(const char*, char*) %s", error_msg);
    }

    received_data = (char*) realloc (received_data, response_message.len);
    received_data = response_message.ptr;

    curl_easy_reset(curl);

    return 1;
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
