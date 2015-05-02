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

#include <pthread.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>

#include "util.h"
#include "thread_handler.h"
#include "excess_main.h"

#include <publisher.h>

// configuration
char* confFile;

/**
 * @brief timestamp for config file
 */
struct timespec timeStampFile = { 0, 0 };

int hostChanged = 0;
int pwd_is_set = 0;

char *pwd;
struct tm *time_info;
FILE *logFile;
char server_name[256];
char name[300];

static void set_pwd();

/**
 * @brief return and print length of excution id
 */
size_t get_stream_data(void *ptr, size_t size, size_t count, void *stream) {
	size_t total = size * count;

	printf("\n--> Sent Output: %s -- len = %d", (char*) ptr, (int) total);
	memcpy(stream, ptr, total);
	return total;
}

/**
 * @brief extract path folder of executable from it's path
 */
char* cutPwd(char *pwd) {
	char *help = malloc(300 * sizeof(char));
	memset(help, '\0', 300 * sizeof(char));
//	help[0] = '\0';
	char *lastslash = strrchr(pwd, '/');
	int ptr = lastslash - pwd;

	memcpy(help, pwd, ptr);

	return help;
}

int prepare() {
	if (!pwd_is_set) {
		set_pwd();
	}

	mfp_parse(confFile);

	//char timeArr[80];
	int clk_id = CLOCK_REALTIME;
	struct timespec date;
    clock_gettime(clk_id, &date);

	//strftime(timeArr, 80, "%c", time_info);


	char str[1000] = ""; /* storing the execution ID -- UUID is 36 chars */
	char msg[1000] = "";

	char *hostname = (char*) malloc(sizeof(char) * 256);
    char *username = getenv("USER");
    if (username == NULL) {
        username = malloc(sizeof(char) * 12);
        strcpy(username, "default");
    }
    const char *description = "default start of the mf agent for testing";

	getFQDN(hostname);
	hostname[strlen(hostname) - 1] = '\0';
	sprintf(msg,
	    "{\"Name\":\"%s\", \"Description\":\"%s\", \"Start_date\":\"%ld\", \"Username\":\"%s\"}",
	    hostname, description, date.tv_sec, username
	);

	strcpy(server_name, mfp_get_value("generic", "server"));
	char *execution_id = get_execution_id(server_name, msg);
	strcpy(str, execution_id);
	strcat(server_name, str);

	free(hostname);

	return 1;
}

int createLogFile() {
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
	if (!logFile)
		return 0;
	fprintf(logFile, "Starting at ... %s\n", timeForFile);
	return 1;
}

int writeTmpPID(void) {
	if (!pwd_is_set) {
		set_pwd();
	}

	strcpy(name, pwd);
	strcat(name, "/tmp_pid");

	int pid = getpid();
	FILE *tmpFile = fopen(name, "w");
	fprintf(tmpFile, "%d", pid);
	fclose(tmpFile);

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
int main(int argc, const char* argv[]) {
	set_pwd();

	writeTmpPID();
	createLogFile();

	int pid = getpid();
	fprintf(stderr, "PID is: %d\n", pid);
	fprintf(logFile, "PID is: %d\n", pid);

    /* set default config file */
    confFile = malloc(sizeof(char) * 256);
	sprintf(confFile, "%s/%s", pwd, "../mf_config.ini");
	fprintf(logFile, "Standard configuration set to: %s \n", confFile);

	if (argc > 1) {
		for (int iter = 0; iter < argc; iter++) {
			char *pos;
			fprintf(stderr, "arg #%d is: %s\n", iter, argv[iter]);
			fprintf(logFile, "arg #%d is: %s\n", iter, argv[iter]);
			if ((pos = strstr(argv[iter], "-id="))) {
				fprintf(stderr, "using executing id: %s\n",
						pos + strlen("-id="));
				fprintf(logFile, "using executing id: %s\n",
						pos + strlen("-id="));
				strcpy(execution_id, pos + strlen("-id="));
			}
			if ((pos = strstr(argv[iter], "-hostname="))) {
				strcpy(server_name, pos + strlen("-hostname="));
				hostChanged = 1;
			}
			if ((pos = strstr(argv[iter], "-config="))) {
				strcpy(confFile, pos + strlen("-config="));
				fprintf(logFile, "using configuration file at: %s\n", confFile);
			}
			if ((pos = strstr(argv[iter], "-h"))
					|| (pos = strstr(argv[iter], "-?"))
					|| (pos = strstr(argv[iter], "--help"))) {
			}
		}
	}

	prepare();

	if (!startThreads()) {
		fprintf(stderr, "Couldn't start the threads!\n");
		fprintf(logFile, "Couldn't start the threads!\n");
	}

	fprintf(stderr, "kthxbye!\n");
	fprintf(logFile, "Program terminated regularly!\n");
	fclose(logFile);
	unlink(name);

	free(confFile);

	exit(EXIT_SUCCESS);
}
