/*
 * thread_handler.c
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>

#include "excess_main.h"
#include "thread_handler.h"

#include "plugin_manager.h"
#include "plugin_discover.h"

int running;

static PluginManager *pm;

pthread_t threads[256];

char execID_[ID_SIZE] = ""; /* storing the execution ID -- UUID is 36 chars */
struct curl_slist *headers_ = NULL;
CURL *curl_ = NULL;

void catcher(int signo) {
	running = 0;
	printf("Signal %d catched\n", signo);

}

int startThreads() {
	int t;
	running = 1;

	pm = PluginManager_new();
	const char *dirname = { "plugins" };

//	look for plugins and register them
	void* pdstate = discover_plugins(dirname, pm);

	int iret[MIN_THREADS + pluginCount];

	apr_initialize();
	apr_pool_create(&data_pool, NULL);

	apr_queue_create(&data_queue, 10e4, data_pool);
	int nums[MIN_THREADS + pluginCount];
	for (t = 0; t < (MIN_THREADS + pluginCount); t++) {
		nums[t] = t;
		iret[t] = pthread_create(&threads[t], NULL, entryThreads, &nums[t]);
		if (iret[t]) {
			fprintf(stderr, "ERROR; return code from pthread_create() is %d\n",
					iret[t]);
			exit(-1);
		}
	}

	struct sigaction sig;
	sig.sa_handler = catcher;
	sig.sa_flags = SA_RESTART;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGTERM, &sig, NULL );
	while (running)
		;

	for (t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL );
	}

//	metric returnMetric = PluginManager_apply_hook(pm);

	cleanup_plugins(pdstate);
	cleanup_curl();
	return 1;
}

void *entryThreads(void *arg) {

	int *typeT = (int*) arg;

	switch (*typeT) {
	case 0:
		startSending();
		break;
	case 1:
		checkConf();
		break;
	default:
		gatherMetric();
		break;
	}

	return NULL ;
}

int startSending() {
	apr_status_t status;
	void *ptr;

	while (running) {
		sleep(timings[0]);

		status = apr_queue_pop(data_queue, &ptr);
		if (status == APR_SUCCESS) {
			metric mPtr = ptr;
			prepSend(mPtr);
		}
	}
	return 1;

}
int send_monitoring_data(char *URL, char *data) {
	CURLcode res;
	int result = SEND_SUCCESS;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"send_monitoring_data(): Error - the given url is empty.\n");
		return SEND_FAILED;
	}

	if (curl_ == NULL ) {
		init_curl();
	}

	printf("curl -X POST %s -- len: %d\n", URL, (int) strlen(URL));
	printf("Msg = %s -- len: %d\n", data, (int) strlen(data));

	curl_easy_setopt(curl_, CURLOPT_URL, URL);
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, (long ) strlen(data));

	res = curl_easy_perform(curl_);

	/* Check for errors */
	if (res != CURLE_OK) {
		result = SEND_FAILED;
		fprintf(stderr, "send_monitoring_data() failed: %s\n",
				curl_easy_strerror(res));

	}

	curl_easy_reset(curl_);
	return result;
}
int prepSend(metric data) {

	char msg[500] = "";
	long double timeStamp = data->timestamp.tv_sec
			+ (long double) (data->timestamp.tv_nsec / 10e8);

	sprintf(msg, "{\"Timestamp\":\"%.9Lf\"%s}", timeStamp, data->msg);
	send_monitoring_data(addr, msg);

	return 1;
}

int gatherMetric() {
	apr_status_t status;
	PluginHook hook = PluginManager_get_hook(pm);
	metric resMetric = malloc(sizeof(metric));

	while (running) {
		resMetric = hook();
		status = apr_queue_push(data_queue, resMetric);
		if (status != APR_SUCCESS)
			fprintf(stderr, "Failed queue push");
	}

	return 1;
}

void init_curl() {
	if (curl_ != NULL ) {
		return;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	curl_ = curl_easy_init();

	headers_ = curl_slist_append(headers_, "Accept: application/json");
	headers_ = curl_slist_append(headers_, "Content-Type: application/json");
	headers_ = curl_slist_append(headers_, "charsets: utf-8");
}

void cleanup_curl() {
	curl_easy_cleanup(curl_);
	curl_slist_free_all(headers_); /* free the header list */
	curl_global_cleanup();
}

int checkConf() {
	while (running) {
		readConf(confFile);
		sleep(timings[1]);
	}
	return 1;
}
