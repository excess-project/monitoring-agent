/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
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

#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
/* start of monitoring-related includes */
#include <publisher.h>
#include "excess_main.h"
#include "thread_handler.h"
#include "plugin_manager.h"
#include "plugin_discover.h"
#include "util.h"

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

int running;

static PluginManager *pm;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[256];

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static void init_timings();

void
catcher(int signo) {
	running = 0;
	printf("\nSignal %d catched\n", signo);
}
static void init_timings();

int
startThreads() {
	//void *ptr;
	int t;
	running = 1;

        pm = PluginManager_new();
	const char *dirname = { "/plugins" };
	char *pluginLocation = malloc(300 * sizeof(char));
	strcpy(pluginLocation, pwd);
	strcat(pluginLocation, dirname);

	void* pdstate = discover_plugins(pluginLocation, pm);
	init_timings();

	int iret[MIN_THREADS + pluginCount];

	apr_initialize();
	apr_pool_create(&data_pool, NULL);

	apr_queue_create(&data_queue, 10e4, data_pool);
	int nums[MIN_THREADS + pluginCount];
	for (t = 0; t < (MIN_THREADS + pluginCount); t++) {
		nums[t] = t;
		iret[t] = pthread_create(&threads[t], NULL, entryThreads, &nums[t]);
		if (iret[t]) {
			fprintf(stderr,
					"ERROR; return code from pthread_create() is %d\n",
					iret[t]);
			fprintf(logFile,
					"ERROR; return code from pthread_create() is %d\n",
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

	/* send the remaining data to the database */
	/*
	while ((apr_queue_trypop(data_queue, &ptr) != APR_EAGAIN)) {
		metric mPtr = ptr;
		prepSend(mPtr);
		free(mPtr);
	}
	*/

	for (t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL );
	}

	cleanup_plugins(pdstate);
	shutdown_curl();
	PluginManager_free(pm);
	apr_queue_term(data_queue);
	apr_pool_destroy(data_pool);

	apr_terminate();
	return 1;
}

void*
entryThreads(void *arg) {
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
	return NULL;
}

int
startSending() {
	void *ptr;
	char update_interval[20] = {'\0'};
	mfp_get_value("timings", "publish_data_interval", update_interval);

	while (running) {
		sleep(atoi(update_interval));
		if (apr_queue_pop(data_queue, &ptr) == APR_SUCCESS) {
			metric mPtr = ptr;
			int retval = prepSend(mPtr);
			if (retval == -1) {
				running = 0;
			}
			free(mPtr);
		}
	}

	return 1;
}

int connection_error = 0;

int
prepSend(metric data) {
	if (!data) {
		return 0;
	}

	/* get timestamp */
	char fmt[64], buf[64];
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    if((tm = localtime(&tv.tv_sec)) != NULL) {
		// yyyy-MM-dd’T'HH:mm:ss.SSS
		strftime(fmt, sizeof fmt, "%Y-%m-%dT%H:%M:%S.%%6u", tm);
		snprintf(buf, sizeof buf, fmt, tv.tv_usec);
    }
    char time_stamp[64];
    memcpy(time_stamp, buf, strlen(buf) - 3);
    time_stamp[strlen(buf) - 3] = '\0';

    /* replace whitespaces in timestamp: yyyy-MM-dd’T'HH:mm:ss. SS */
    int i = 0;
  	while (time_stamp[i]) {
	    if (isspace(time_stamp[i])) {
    	    time_stamp[i] = '0';
	    }
    	i++;
  	}

	char msg[4096] = "";
	sprintf(msg,
		"{\"@timestamp\":\"%s\",\"host\":\"%s\",\"task\":\"%s\",%s}",
		time_stamp,
		hostname,
		task,
		data->msg
	);

	int retval = publish_json(server_name, msg);
	if (retval == 0) {
		++connection_error;
	}
	if (connection_error == 50) {
		fprintf(stderr, "ERROR: Too many connection errors: %s\n", server_name);
		return -1;
	}

	return 1;
}

long timings[256];

static void
init_timings()
{
	mfp_data *mfp_timing_data = malloc(sizeof(mfp_data));
	mfp_get_data("timings", mfp_timing_data);

	char timing[20] = {'\0'};
	mfp_get_value("timings", "publish_data_interval", timing);
	timings[0] = atoi(timing);

	memset(timing, 0, sizeof(timing));
	mfp_get_value("timings", "update_configuration", timing);
	timings[1] = atoi(timing);

	memset(timing, 0, sizeof(timing));
	mfp_get_value("timings", "default", timing);
	long default_timing = atoi(timing);

	for (int i = 2; i < mfp_timing_data->size; ++i) {
		char* current_plugin_name = plugin_name[i];
		if (current_plugin_name == NULL) {
			continue;
		}
		char value[20] = {'\0'};
		mfp_get_value("timings", current_plugin_name, value);
		if (value[0] == '\0') {
			timings[i] = default_timing;
		} else {
			timings[i] = atoi(value);
			fprintf(stderr,
					"\ntiming for plugin %s is %ld\n",
					current_plugin_name, timings[i]
			);
		}
	}

	free(mfp_timing_data);
}

int
gatherMetric(int num) {
	char* current_plugin_name = plugin_name[num];

	struct timespec tim = { 0, 0 };
	struct timespec tim2;

	if (timings[num] >= 10e8) {
		tim.tv_sec = timings[num] / 10e8;
		tim.tv_nsec = timings[num] % (long) 10e8;
	} else {
		tim.tv_sec = 0;
		tim.tv_nsec = timings[num];
	}
	PluginHook hook = PluginManager_get_hook(pm);
	fprintf(stderr,
			"\ngather metric %s (#%d) with update interval of %ld ns\n",
			current_plugin_name, num, timings[num]
	);
	fprintf(logFile,
			"\ngather metric %s (#%d) with update interval of %ld ns\n",
			current_plugin_name, num, timings[num]
	);
	metric resMetric = malloc(sizeof(metric_t));

	while (running) {
		resMetric = hook();
		if (apr_queue_push(data_queue, resMetric) != APR_SUCCESS) {
			fprintf(stderr, "Failed queue push");
			fprintf(logFile, "failed queue push");
		}
		nanosleep(&tim, &tim2);
	}
	free(resMetric);

	/* call when terminating program, enables cleanup of plug-ins */
	hook();

	return 1;
}

int
checkConf() {
	while (running) {
		mfp_parse(confFile);
        init_timings();
        char wait_some_seconds[20] = {'\0'};
        mfp_get_value("timings", "update_configuration", wait_some_seconds);
		sleep(atoi(wait_some_seconds));
	}

	return 1;
}
