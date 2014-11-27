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

#include <publisher.h>

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

void catcher(int signo) {
	running = 0;
	printf("\nSignal %d catched\n", signo);

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
	shutdown_curl();
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
		char* update_interval = mfp_get_value("timings", "publish_data_interval");
		sleep(atoi(update_interval));

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

void removeSpace(char *str) {
	char *p1 = str, *p2 = str;
	do
		while (*p2 == ' ')
			p2++;
	while ( (*p1++ = *p2++) );
}

int prepSend(metric data) {

	char msg[4096] = "";
	long double timeStamp = data->timestamp.tv_sec
			+ (long double) (data->timestamp.tv_nsec / 10e8);

	char *hostname = (char*) malloc(sizeof(char) * 80);
	getFQDN(hostname);
	hostname[strlen(hostname) - 1] = '\0';

	sprintf(msg, "{\"Timestamp\":%.9Lf,\"hostname\":\"%s\"%s}", timeStamp, hostname, data->msg);
	publish_json(server_name, msg);
	free(data);
	free(hostname);

	return 1;
}

static mfp_data *mfp_timing_data;
long timings[256];

static void init_timings()
{
	mfp_data *mfp_timing_data = malloc(sizeof(mfp_data));

	mfp_get_data("timings", mfp_timing_data);
	char* timing = mfp_get_value("timings", "publish_data_interval");
	timings[0] = atoi(timing);
	timing = mfp_get_value("timings", "update_configuration");
	timings[1] = atoi(timing);

	// FIXME: just set default timing for the rest of the plugins
	long default_timing = atoi(mfp_get_value("timings", "default"));
	for (int i = 2; i < mfp_timing_data->size; ++i) {
		timings[i] = default_timing;
	}

	free(mfp_timing_data);
}

int gatherMetric(int num) {
	char name[50];
	sprintf(name, "gatherMetricNo%d", num);
//	startStop(name, START);
	struct timespec tim = { 0, 0 };
	struct timespec tim2;

	init_timings(); // FIXME: just a work-around

	if (timings[num] >= 10e8) {
		tim.tv_sec = timings[num] / 10e8;
		tim.tv_nsec = timings[num] % (long) 10e8;
	} else {
		tim.tv_sec = 0;
		tim.tv_nsec = timings[num];
	}

	apr_status_t status;
	PluginHook hook = PluginManager_get_hook(pm);
	fprintf(stderr,  "with timing: %ld ns\n", timings[num]);
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
	
	//startStop(name, STOP);

	return 1;
}

int checkConf() {
	while (running) {
		mfp_parse(confFile);

		char *wait_some_seconds = mfp_get_value("timings", "update_configuration");
		sleep(atoi(wait_some_seconds));
	}
	return 1;
}
