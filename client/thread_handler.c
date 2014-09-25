/*
 * thread_handler.c
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "excess_main.h"
#include "thread_handler.h"

#include "plugin_manager.h"
#include "plugin_discover.h"

#include "util.h"

int running;

static PluginManager *pm;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t threads[256];

char execID_[ID_SIZE] = ""; /* storing the execution ID -- UUID is 36 chars */
struct curl_slist *headers_ = NULL;
CURL *curl_ = NULL;

void catcher(int signo) {
	running = 0;
	printf("\nSignal %d catched\n", signo);

}

int startStop(const char *fnctName, int flag) {
	metric resMetric = malloc(sizeof(metric_t));

	resMetric->msg = malloc(100 * sizeof(char));

	int clk_id = CLOCK_REALTIME;
	clock_gettime(clk_id, &resMetric->timestamp);

//	&resMetric->timestamp=

	sprintf(resMetric->msg, ",\"name\":\"%s\",\"status\":\"%d\"", fnctName,
			flag);

	apr_status_t status = apr_queue_push(data_queue, resMetric);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "Failed queue push");
		fprintf(logFile, "Failed queue push");
	}
//	free(resMetric->msg);
//	free(resMetric);
	return 1;
}

int startThreads() {
	int t;
	running = 1;

	pm = PluginManager_new();
	const char *dirname = { "/plugins" };
	char *pluginLocation = malloc(300 * sizeof(char));
	strcpy(pluginLocation, pwd);
	strcat(pluginLocation, dirname);

//	look for plugins and register them
	void* pdstate = discover_plugins(pluginLocation, pm);

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
			fprintf(logFile, "ERROR; return code from pthread_create() is %d\n",
					iret[t]);
			exit(-1);
		}
	}

	struct sigaction sig;
	sig.sa_handler = catcher;
	sig.sa_flags = SA_RESTART;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGTERM, &sig, NULL );
	sigaction(SIGINT, &sig, NULL );
	while (running)
		sleep(1);

	for (t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL );
	}

//	metric returnMetric = PluginManager_apply_hook(pm);

	cleanup_plugins(pdstate);
	cleanup_curl();
	PluginManager_free(pm);
	apr_queue_term(data_queue);
	apr_pool_destroy(data_pool);

	apr_terminate();
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
		gatherMetric(*typeT);
		break;
	}

	return NULL ;
}

int startSending() {
//	startStop("startSending", START);
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
	while ((apr_queue_pop(data_queue, &ptr) == APR_SUCCESS)) {
		metric mPtr = ptr;
		prepSend(mPtr);
	}
	return 1;

}

static size_t write_data(void *data, size_t blksz, size_t nblk, void *ctx) {
	static size_t sz = 0;
	size_t currsz = blksz * nblk;

	size_t prevsz = sz;
	sz += currsz;
	void *tmp = realloc(*(char **) ctx, sz);
	if (tmp == NULL ) {
		// handle error
		free(*(char **) ctx);
		*(char **) ctx = NULL;
		return 0;
	}
	*(char **) ctx = tmp;

	memcpy(*(char **) ctx + prevsz, data, currsz);
	return currsz;
}

void removeSpace(char *str) {
	char *p1 = str, *p2 = str;
	do
		while (*p2 == ' ')
			p2++;
	while (*p1++ = *p2++);
}
char* queryRangeFromDB(const char *URL, const char *id, long double t0,
		long double t1) {

	char data[300] = { '\0' };
	char *response = malloc(100000 * sizeof(char));
	memset(response, 100000, '\0');
	sprintf(data, "%s/executions/%s/%.9Lf/%.9Lf", URL, id, t0, t1);

	CURLcode res;
	int result = SEND_SUCCESS;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"send_monitoring_data(): Error - the given url is empty.\n");
		fprintf(logFile,
				"send_monitoring_data(): Error - the given url is empty.\n");
		return SEND_FAILED;
	}

	if (curl_ == NULL ) {
		init_curl();
	}

	printf("curl -X GET %s -- len: %d\n", data, (int) strlen(data));
	printf("Msg = %s -- len: %d\n", data, (int) strlen(data));

	curl_easy_setopt(curl_, CURLOPT_URL, data);
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
	curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1L);

	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

	res = curl_easy_perform(curl_);

	/* Check for errors */
	if (res != CURLE_OK) {
		result = SEND_FAILED;
		fprintf(stderr, "send_monitoring_data() failed: %s\n",
				curl_easy_strerror(res));
		fprintf(logFile, "send_monitoring_data() failed: %s\n",
				curl_easy_strerror(res));

	}

	curl_easy_reset(curl_);
	if (result != SEND_SUCCESS) {
		return NULL ;
	}

	// lets trim the result
	for (int ind = 0; ind < strlen(response); ind++)
		if (response[ind] == '\n')
			response[ind] = ' ';
	removeSpace(response);
	return response;
}

int send_monitoring_data(char *URL, char *data) {
	CURLcode res;
	int result = SEND_SUCCESS;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"send_monitoring_data(): Error - the given url is empty.\n");
		fprintf(logFile,
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
		fprintf(logFile, "send_monitoring_data() failed: %s\n",
				curl_easy_strerror(res));

	}

	curl_easy_reset(curl_);
	return result;
}
int prepSend(metric data) {

	char msg[500] = "";
	long double timeStamp = data->timestamp.tv_sec
			+ (long double) (data->timestamp.tv_nsec / 10e8);

	sprintf(msg, "{\"Timestamp\":%.9Lf%s}", timeStamp, data->msg);
	send_monitoring_data(addr, msg);
	free(data);

	return 1;
}

int gatherMetric(int num) {
	char name[50];
	sprintf(name, "gatherMetricNo%d", num);
//	startStop(name, START);
	struct timespec tim = { 0, 0 };
	struct timespec tim2;
	if (timings[num] >= 10e8) {
		tim.tv_sec = timings[num] / 10e8;
		tim.tv_nsec = timings[num] % (long) 10e8;
	} else {
		tim.tv_sec = 0;
		tim.tv_nsec = timings[num];
	}

	apr_status_t status;
	PluginHook hook = PluginManager_get_hook(pm);
	fprintf(stderr, "with timing: %ld ns\n", timings[num]);
	fprintf(logFile, "with timing: %ld ns\n", timings[num]);
	metric resMetric = malloc(sizeof(metric_t));

	while (running) {
		resMetric = hook();
		status = apr_queue_push(data_queue, resMetric);
		if (status != APR_SUCCESS) {
			fprintf(stderr, "Failed queue push");
			fprintf(logFile, "failed queue push");
		}
		nanosleep(&tim, &tim2);
	}
	hook(); // call when terminating programm, enables cleanup of plugins
	free(resMetric);
	startStop(name, STOP);
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
