#include <curl/curl.h>
#include <string.h>

#include "debug.h"
#include "publisher.h"

static CURL *curl;
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
static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}
#endif

int check_URL(const char *URL)
{
    if (URL == NULL || *URL == '\0') {
        char *error_msg = "URL not set.";
		log_error("publish(const char*, Message) %s", error_msg);
		return 0;
	}
	return 1;
}

int check_message(char *message)
{
	if (message == NULL || *message == '\0') {
	    char *error_msg = "message not set.";
		log_error("publish(const char*, Message) %s", error_msg);
		return 0;	    
	}
	return 1;
}

int publish_json(const char *URL, char *message)
{
	int result = SEND_SUCCESS;
	CURLcode response;

    if (!check_URL(URL) || !check_message(message)) {
        return 0;
    }

    init_curl();
	
	curl_easy_setopt(curl, CURLOPT_URL, URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long ) strlen(message));
	#ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	#else
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	#endif

	response = curl_easy_perform(curl);
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
    
    return "lnWrd10tQFmR30ekCjkAuQ";
}

void shutdown_curl()
{
    if (curl == NULL ) {
		return;
	}
	
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
