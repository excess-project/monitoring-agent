/*
 * Copyright 2014-2016 High Performance Computing Center, Stuttgart
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
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <publisher.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
/* monitoring related includes */
#include "excess_main.h"
#include "thread_handler.h"
#include "util.h"

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

char* confFile;
char* experiment_id;
char* workflow;
char* task;
char* hostname;
char* api_version;
double publish_json_time;

int hostChanged = 0;
int pwd_is_set = 0;

char *pwd;
struct tm *time_info;
FILE *logFile;
char server_name[256];
char name[300];

extern int errno;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static void set_pwd();

static void
toLower(char* word, int length)
{
    int i;
    for(i = 0; i < length; i++) {
        word[i] = tolower(word[i]);
    }
}

/* Perform initialization prior starting monitoring */
int
prepare() {
	if (!pwd_is_set) {
		set_pwd();
	}
	/* get server */
	mfp_get_value("generic", "server", server_name);

	/* prepare default message */
	char msg[1000] = "";

	/* get timestamp */
	char time_stamp[64] = {'\0'};
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	double timestamp = ts.tv_sec + (double)(ts.tv_nsec / 1.0e9);
	convert_time_to_char(timestamp, time_stamp);

	/* set workflow and task_id to lower case */
	toLower(workflow, strlen(workflow));
	toLower(task, strlen(task));

	sprintf(msg,
		"{\"host\":\"%s\",\"@timestamp\":\"%s\",\"user\":\"%s\",\"application\":\"%s\",\"job_id\":\"non-pbs\"}",
		hostname, time_stamp, workflow, task
	);

	/* set or create experiment id */
	struct timespec ts_start, ts_end;
	clock_gettime(CLOCK_REALTIME, &ts_start);
	if ((experiment_id != NULL) && (experiment_id[0] == '\0')) {
		/*we don't have an experiment_id */
		char* URL = malloc(256 * sizeof(char));
		sprintf(URL,
				"%s/%s/mf/users/%s/create",
				server_name, api_version, workflow
		);
		strcpy(experiment_id, create_experiment_id(URL, msg));
		fprintf(logFile, "> experiment ID: %s\n", experiment_id);
		free(URL);

		if (strstr(experiment_id, "error") != NULL || experiment_id[0] == '\0') {
    		fprintf(stderr,
    				"ERROR. Could not create experiment ID: %s\n",
    				experiment_id
    		);
    		return 0;
		}
	} else { 
		/* we have already an experiment_id, register it at the server if no such experiment exists */
		char* URL = malloc(256 * sizeof(char));
		sprintf(URL,
				"%s/%s/mf/users/%s/%s/create",
				server_name, api_version, workflow, experiment_id
		);
		publish_json(URL, msg);
		free(URL);
	}
	clock_gettime(CLOCK_REALTIME, &ts_end);
	publish_json_time = (ts_end.tv_sec - ts_start.tv_sec) + (double)((ts_end.tv_nsec - ts_start.tv_nsec) / 1.0e9);
	/* set the correct path for sending metric data */
	char* path = malloc(256 * sizeof(char));
	sprintf(path, "/%s/mf/metrics/", api_version);
	strcat(server_name, path);
	free(path);

	return 1;
}

/* Initializing and creating the log file */
void
createLogFile() {
	int errnum;

	if (!pwd_is_set) {
		set_pwd();
	}
	char logFileName[300] = { '\0' };
	char logFileFolder[300] = { '\0' };

	time_t curTime;
	time(&curTime);
	time_info = localtime(&curTime);

	char timeForFile[50];

	strftime(timeForFile, 50, "%F-%T", time_info);
	sprintf(logFileFolder, "%s/log", pwd);
	sprintf(logFileName, "%s/log/log-%s", pwd, timeForFile);
	fprintf(stderr, "using logfile: %s\n", logFileName);

	struct stat st = { 0 };
	if (stat(logFileFolder, &st) == -1)
		mkdir(logFileFolder, 0700);
	logFile = fopen(logFileName, "w");
	if (logFile == NULL) {
		errnum = errno;
		fprintf(stderr, "Could not create log file: %s\n", name);
		fprintf(stderr, "Error creating log: %s\n", strerror(errnum));
	} else {
		fprintf(logFile, "Starting at ... %s\n", timeForFile);
	}
}

/* write PID to a file in `pwd`; used by PBS to kill the agent */
int
writeTmpPID(void) {
	int errnum;

	if (!pwd_is_set) {
		set_pwd();
	}

	strcpy(name, pwd);
	strcat(name, "/tmp_pid");

	int pid = getpid();
	FILE *tmpFile = fopen(name, "w");
	if (tmpFile == NULL) {
		errnum = errno;
		fprintf(stderr, "Error creating file: %s\n", strerror(errnum));
		fprintf(stderr, "Could not create file to store PID: %s\n", name);
	} else {
		fprintf(tmpFile, "%d", pid);
		fclose(tmpFile);
	}

	fprintf(stderr, "PID is: %d\n", pid);
	if (logFile != NULL) {
		fprintf(logFile, "PID is: %d\n", pid);
	}

	return 1;
}

/* assigns the current working directory to a variable */
static void
set_pwd()
{
	if (pwd_is_set) {
		return;
	}
	char buf_1[300] = {'\0'};
	char buf_2[300] = {'\0'};
	
	readlink("/proc/self/exe", buf_1, 200);
	memcpy(buf_2, buf_1, strlen(buf_1) * sizeof(char));

	/* extract path folder of executable from it's path */
	char *lastslash = strrchr(buf_2, '/');
	int ptr = lastslash - buf_2;

	memcpy(pwd, buf_2, ptr);
	pwd_is_set = 1;
}

/* everything starts here */
int main(int argc, char* argv[]) {
	int c;
	int h_flag = 0; //arg "hostname" exists flag
	int w_flag = 0;	//arg "workflow" exists flag
	int t_flag = 0;	//arg "task" exists flag
	int c_flag = 0;	//arg "config file" exists flag
	int a_flag = 0;	//arg "api version" exists flag
	int err = 0, help = 0;
	extern char *optarg;

	/* assigns the current working directory to a variable */
	pwd = (char *)calloc(300, sizeof(char));
	set_pwd();

	/* creates the log files used for this execution */
	createLogFile();

	/* write PID to a file in `pwd`; used by PBS to kill the agent */
	writeTmpPID();

	/* init arguments */
	hostname = malloc(256 * sizeof(char));
	workflow = malloc(256 * sizeof(char));
	task = malloc(128 * sizeof(char));
	experiment_id = malloc(128 * sizeof(char));
	confFile = malloc(256 * sizeof(char));
	api_version = malloc(32 * sizeof(char));

	hostname[0] = '\0';
	workflow[0] = '\0';
	task[0] = '\0';
	experiment_id[0] = '\0';
	confFile[0] = '\0';
	api_version[0] = '\0';

	/* parse command-line arguments */
	static char usage[] = "usage: %s [-o hostname] [-e id] [-w workflow] [-t task] [-c config] [-a version] [-h help]\n";
	while ((c = getopt(argc, argv, "o:w:t:c:e:a:h")) != -1)
		switch (c) {
		case 'o':
			strcpy(hostname, optarg);
			h_flag = 1;
			fprintf(logFile, "> hostname: %s\n", hostname);
			break;
		case 'w':
			strcpy(workflow, optarg);
			w_flag = 1;
			fprintf(logFile, "> workflow ID: %s\n", workflow);
			break;
		case 't':
			strcpy(task, optarg);
			t_flag = 1;
			fprintf(logFile, "> task ID: %s\n", task);
			break;
		case 'c':
			strcpy(confFile, optarg);
			c_flag = 1;
			fprintf(logFile, "> config file: %s\n", confFile);
			break;
		case 'e':
			strcpy(experiment_id, optarg);
			fprintf(logFile, "> experiment ID: %s\n", experiment_id);
			break;
		case 'a':
			strcpy(api_version, optarg);
			a_flag = 1;
			fprintf(logFile, "> api version: %s\n", api_version);
			break;
		case 'h':
			help = 1;
			break;
		case '?':
			err = 1;
			break;
	}
	
	/* retrieve hostname if not given as input arg */
	if (h_flag == 0) {
		getFQDN(hostname);
		hostname[strlen(hostname) - 1] = '\0';
	}
	
	/* set workflow to "username" if not given as input arg */
	if (w_flag == 0) {
		/* get workflow id by environment "USER" */
		if (getenv("USER") == NULL) {
			strcpy(workflow, "unknown");
		}
		else {
			strcpy(workflow, getenv("USER"));	
		}
	}
	
	/* set task to all if not provided by the user */
	if (t_flag == 0) {
		strcpy(task, "manual_monitoring");
	}
	
	/* set default configuration file if no configuration was given */
	if (c_flag == 0) {
		sprintf(confFile, "%s/%s", pwd, "../mf_config.ini");
	}
	fprintf(logFile, "Configuration taken from: %s\n", confFile);
	
	/* set default api version */
	if (a_flag == 0) {
		strcpy(api_version, "v1");
	}
	/* print usage */
	if (err || help) {
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	/* try to parse configuration file */
	if (mfp_parse(confFile) == 0) {
		/* could not parse configuration file */
		fprintf(logFile, "Stopping service...could not parse configuration.\n");
		exit(1);
	}

	/* setup URLs to monitoring server */
	if (prepare() == 1) {
		/* start the monitoring threads */
		int retval = startThreads();
		if (retval == 0) {
			fprintf(logFile, "Couldn't start the threads!\n");
		}
	}

	/* clean up tasks */
	fprintf(logFile, "Stopping service ...\n");

	fclose(logFile);
	unlink(name);

	free(pwd);
	free(confFile);
	mfp_parse_clean();
	free(workflow);
	free(task);
	free(experiment_id);
	free(hostname);

	exit(EXIT_SUCCESS);
}
