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

/**
 * @brief extract path folder of executable from it's path
 */
char*
cutPwd(char *pwd) {
	char *help = malloc(300 * sizeof(char));
	memset(help, '\0', 300 * sizeof(char));
	char *lastslash = strrchr(pwd, '/');
	int ptr = lastslash - pwd;

	memcpy(help, pwd, ptr);

	return help;
}

static void
toLower(char* word, int length)
{
    int i;
    for(i = 0; i < length; i++) {
        word[i] = tolower(word[i]);
    }
}

int
prepare() {
	if (!pwd_is_set) {
		set_pwd();
	}

	/* prepare default message */
	char msg[1000] = "";

	/* retrieve hostname */
	getFQDN(hostname);
	hostname[strlen(hostname) - 1] = '\0';

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

    /* get username */
    char *username = getenv("USER");
    if (username == NULL) {
		username = malloc(sizeof(char) * 128);
		strcpy(username, "unknown");
    }

	/* set task id */
	toLower(task, strlen(task));

    /* set default description */
    const char *description = "Running with default configuration...";

	sprintf(msg,
		"{\"host\":\"%s\",\"description\":\"%s\",\"@timestamp\":\"%s\",\"user\":\"%s\",\"application\":\"%s\"}",
		hostname, description, time_stamp, username, task
	);

	/* get server */
	mfp_get_value("generic", "server", server_name);

	/* set workflow id */
	if ((workflow != NULL) && (workflow[0] == '\0')) {
		strcpy(workflow, username);
	}
	toLower(workflow, strlen(workflow));

	/* set or create experiment id */
	if ((experiment_id != NULL) && (experiment_id[0] == '\0')) {
		char* URL = malloc(256 * sizeof(char));
		sprintf(URL,
				"%s/%s/mf/users/%s/create",
				server_name, api_version, workflow
		);
		strcpy(experiment_id, create_experiment_id(URL, msg));
		free(URL);

		if (strstr(experiment_id, "error") != NULL || experiment_id[0] == '\0') {
    		fprintf(stderr,
    				"ERROR. Could not create experiment ID: %s\n",
    				experiment_id
    		);
    		return 0;
		}
	}

	/* set the correct path for sending metric data */
	char* path = malloc(256 * sizeof(char));
	sprintf(path,
			"/%s/mf/metrics/%s/%s?task=%s",
			api_version, workflow, experiment_id, task
	);
	strcat(server_name, path);

	return 1;
}

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

static void
set_pwd()
{
	if (pwd_is_set) {
		return;
	}

	char *buf = malloc(300 * sizeof(char));
	memset(buf, '\0', 300 * sizeof(char));

	readlink("/proc/self/exe", buf, 200);

	pwd = malloc(300 * sizeof(char));
	memset(pwd, '\0', 300 * sizeof(char));
	memcpy(pwd, buf, strlen(buf) * sizeof(char));

	pwd = cutPwd(pwd);

	free(buf);
	pwd_is_set = 1;
}

/**
 * @brief everything starts here
 */
int main(int argc, char* argv[]) {
	int c;
	int a_flag = 0;
	int c_flag = 0;
	int t_flag = 0;
	int err = 0, help = 0;
	extern char *optarg;

	/* assigns the current working directory to a variable */
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
	static char usage[] = "usage: %s [-e id] [-w workflow] [-t task] [-c config] [-a version] [-h help]\n";
	while ((c = getopt(argc, argv, "w:t:c:e:h")) != -1)
		switch (c) {
		case 'w':
			strcpy(workflow, optarg);
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
			fprintf(logFile, "> experiment ID: %s\n", confFile);
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

	/* set default configuration file if no configuration was given */
	if (c_flag == 0) {
		sprintf(confFile, "%s/%s", pwd, "../mf_config.ini");
	}
	fprintf(logFile, "Configuration taken from: %s\n", confFile);

	/* set task to all if not provided by the user */
	if (t_flag == 0) {
		strcpy(task, "manual_monitoring");
	}

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
		fprintf(stderr,  "Stopping service...could not parse configuration.\n");
		fprintf(logFile, "Stopping service...could not parse configuration.\n");
		exit(1);
	}

	/* setup URLs to monitoring server */
	if (prepare() == 1) {
		/* start the monitoring threads */
		int retval = startThreads();
		if (retval == 0) {
			fprintf(stderr, "Couldn't start the threads!\n");
			fprintf(logFile, "Couldn't start the threads!\n");
		}
	}

	/* clean up tasks */
	fprintf(stderr,  "Stopping service ...\n");
	fprintf(logFile, "Stopping service ...\n");

	fclose(logFile);
	unlink(name);

	free(pwd);
	free(confFile);
	free(workflow);
	free(task);
	free(experiment_id);
	free(hostname);

	exit(EXIT_SUCCESS);
}
