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
#include <time.h>
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
long timings[256];	//defined as extern in excess_main.h
long min_plugin_interval;
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
	int t;
	running = 1;

        pm = PluginManager_new();
	const char *dirname = { "/plugins" };
	char *pluginLocation = malloc(300 * sizeof(char));
	strcpy(pluginLocation, pwd);
	strcat(pluginLocation, dirname);

	void* pdstate = discover_plugins(pluginLocation, pm);
	init_timings();
	//calculate the sending threads number
	int sending_threads = (int) (pluginCount * publish_json_time * 1.0e9 / (min_plugin_interval * BULK_SIZE));
	int num_threads = MIN_THREADS + pluginCount + sending_threads;
	fprintf(logFile, "\nnumber of plugins is \t\t%d\n", pluginCount);
	fprintf(logFile, "time used for publish json is \t%f(s)\n", publish_json_time);
	fprintf(logFile, "minimum plugin time interval is \t%ld(ns)\n", min_plugin_interval);
	fprintf(logFile, "BULK_SIZE is \t\t%d\n", BULK_SIZE);
	fprintf(logFile, "num of threads of sending is \t%d\n", sending_threads);
	fprintf(logFile, "total number of threads is \t\t%d\n", num_threads);
	int iret[num_threads];
	int nums[num_threads];

	data_queue = ECQ_create(0);
	for (t = 0; t < num_threads; t++) {
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
	
	//thread join from plugins threads till all the sending threads
	for (t = MIN_THREADS; t < num_threads; t++) {
		pthread_join(threads[t], NULL);
	}

	cleanup_plugins(pdstate);
	shutdown_curl();
	PluginManager_free(pm);
	ECQ_free(data_queue);
	return 1;
}

void*
entryThreads(void *arg) {
	int *typeT = (int*) arg;
	if(*typeT == 0) {
		checkConf();
	}
	else if((MIN_THREADS <= *typeT) && (*typeT< MIN_THREADS + pluginCount)) {
		gatherMetric(*typeT);
	}
	else {
		startSending();
	}
	return NULL;
}

int
startSending() {
	void *ptr;
	EXCESS_concurrent_queue_handle_t data_queue_handle;
	data_queue_handle =ECQ_get_handle(data_queue);
	while (running || !ECQ_is_empty(data_queue_handle)) {
		if(ECQ_try_dequeue(data_queue_handle, &ptr)) {
			metric *mPtr = ptr;
			int retval = prepSend(mPtr);
			if (retval == -1) {
				running = 0;
			}
			free_bulk(mPtr, BULK_SIZE);
		}
		else {
			usleep(timings[0]);
		}
	}
	ECQ_free_handle(data_queue_handle);
	return 1;
}

int connection_error = 0;

int
prepSend(metric *data) {
	int i;
	char json[1024 * BULK_SIZE] = {'\0'};
	json[0] = '[';
	if (!data) {
		return 0;
	}
	for(i=0; i<BULK_SIZE && data[i] != NULL; i++) {
		/* use data->timestamp from plugin */
		char time_stamp[64] = {'\0'};
		double timestamp = data[i]->timestamp.tv_sec + (double)(data[i]->timestamp.tv_nsec / 1.0e9);
		convert_time_to_char(timestamp, time_stamp);

		/*
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		double publish_ts = ts.tv_sec + (double)(ts.tv_nsec / 1.0e9);
		double diff_ts = publish_ts - timestamp;
		fprintf(logFile, "diff_ts is \t%f\n", diff_ts);*/
		char msg[1024] = {'\0'};
		sprintf(msg,
			"{\"@timestamp\":\"%s\",\"host\":\"%s\",\"WorkflowID\":\"%s\",\"ExperimentID\":\"%s\",\"task\":\"%s\",%s},",
			time_stamp,
			hostname,
			workflow,
			experiment_id,
			task,
			data[i]->msg
		);
		strcat(json, msg);
	}
	json[strlen(json)-1] = ']';
	json[strlen(json)] = '\0';
	int retval = publish_json(server_name, json);
	if (retval == 0) {
		++connection_error;
	}
	if (connection_error == 50) {
		fprintf(stderr, "ERROR: Too many connection errors: %s\n", server_name);
		return -1;
	}
	return 1;
}

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
	min_plugin_interval = default_timing;

	for (int i = MIN_THREADS; i < MIN_THREADS + mfp_timing_data->size; ++i) {
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
		//get the min_plugin_interval
		if(timings[i] < min_plugin_interval) {
			min_plugin_interval = timings[i];
		}
	}

	free(mfp_timing_data);
}

int
gatherMetric(int num) {
	int i;
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

	EXCESS_concurrent_queue_handle_t data_queue_handle;
	data_queue_handle =ECQ_get_handle(data_queue);
	while (running) {
		//malloc a pointer to bulk of metrics
		metric *resMetrics = (metric *) 0;
		resMetrics = (metric *) malloc(BULK_SIZE * sizeof(metric));
		for(i=0; i<BULK_SIZE; i++) {
			resMetrics[i] = hook();	//malloc of resMetrics[i] in hook()
			//fprintf(logFile, "\n%p\t address of the %dth metric get hook\n", resMetrics[i], i);
			nanosleep(&tim, &tim2);
		}
		ECQ_enqueue(data_queue_handle, (void *)resMetrics);
	}
	ECQ_free_handle(data_queue_handle);
	/* call when terminating program, enables cleanup of plug-ins */
	hook();

	return 1;
}

int
checkConf() {
	while (running) {
		mfp_parse(confFile);
		char wait_some_seconds[20] = {'\0'};
		mfp_get_value("timings", "update_configuration", wait_some_seconds);
		sleep(atoi(wait_some_seconds));
		init_timings();
	}

	return 1;
}
