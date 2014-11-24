/*
 * excess_main.c
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#include <pthread.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>

#include "util.h"
#include "thread_handler.h"
#include "excess_main.h"


// configuration file
char *confFile;
config_generic conf_generic;
config_timings conf_timings;

/**
 * @brief timestamp for config file
 */
struct timespec timeStampFile = { 0, 0 };

int hostChanged = 0;

char *pwd;
struct tm *time_info;
FILE *logFile;
char name[300];

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
 * @brief get execution id from server side or skip if already existent
 */
char* get_execution_id(char *URL, char *msg) {
	CURLcode res;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"get_execution_id(): Error - the given url is empty.\n");
		fprintf(logFile,
				"get_execution_id(): Error - the given url is empty.\n");
		return NULL ;
	}

	if (msg == NULL || strlen(msg) == 0) {
		fprintf(stderr,
				"get_execution_id(): Error - empty message is going to be sent.\n");
		fprintf(logFile,
				"get_execution_id(): Error - empty message is going to be sent.\n");
		return NULL ;
	}

	if (execID_ != NULL && strlen(execID_) > 0) {

		return execID_;
	}

	if (curl_ == NULL ) {
		init_curl();
	}

	printf("\nSending curl -X POST %s -- len: %d\n", URL, (int) strlen(URL));
	printf("Msg = %s -- len: %d\n", msg, (int) strlen(msg));

	curl_easy_setopt(curl_, CURLOPT_URL, URL);
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, msg);
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, (long ) strlen(msg));

	/* get the execution ID from the stream data */
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, get_stream_data);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &execID_);

	res = curl_easy_perform(curl_);
	printf("\n-- Execution ID: %s -- len: %d\n", execID_,
			(int) strlen(execID_));

	if (res != CURLE_OK) {
		fprintf(stderr, "get_execution_id() failed: %s\n",
				curl_easy_strerror(res));
		fprintf(logFile, "get_execution_id() failed: %s\n",
				curl_easy_strerror(res));
		return NULL ;

	}

	curl_easy_reset(curl_);
	return execID_;
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

static int prepare() {
	confFile = malloc(strlen(pwd) + strlen("..") + strlen("mf_config.ini") + 3);
	if (confFile == NULL) {
		fprintf(stderr, "prepare() failed: cannot allocate memory for fullpath");
		return 0;
	}
	sprintf(confFile, "%s/%s/%s", pwd, "..", "mf_config.ini");
	fprintf(logFile, "confFile is: %s \n", confFile);
	parse_generic(confFile, &conf_generic);
	parse_timings(confFile, &conf_timings);

	char timeArr[80];

	strftime(timeArr, 80, "%c", time_info);

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
	    "{\"Name\":\"%s\", \"Description\":\"%s\", \"Start_date\":\"%s\", \"Username\":\"%s\"}",
	    hostname, description, timeArr, username
	);

	/* init curl libs */
	init_curl();

	strcpy(str, get_execution_id(conf_generic.server, msg)); /* get the execution ID */
	strcat(conf_generic.server, str);

	free(hostname);

	return 1;
}

int createLogFile() {
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

	strcpy(name, pwd);
	strcat(name, "/tmp_pid");

	int pid = getpid();
	FILE *tmpFile = fopen(name, "w");
	fprintf(tmpFile, "%d", pid);
	fclose(tmpFile);

	return 1;
}

/**
 * @brief everything starts here
 */
int main(int argc, const char* argv[]) {
	char *buf = malloc(300 * sizeof(char));
	memset(buf, '\0', 300 * sizeof(char));

	readlink("/proc/self/exe", buf, 200); // obtain full path of executable

	pwd = malloc(300 * sizeof(char));
	memset(pwd, '\0', 300 * sizeof(char));
	memcpy(pwd, buf, strlen(buf) * sizeof(char));

	pwd = cutPwd(pwd);
	writeTmpPID();
	createLogFile();

	int pid = getpid();
	fprintf(stderr, "PID is: %d\n", pid);
	fprintf(logFile, "PID is: %d\n", pid);

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
				strcpy(execID_, pos + strlen("-id="));
			}
			if ((pos = strstr(argv[iter], "-hostname="))) {
				strcpy(conf_generic.server, pos + strlen("-hostname=")); // FIXME
				hostChanged = 1;
			}
			if ((pos = strstr(argv[iter], "-h"))
					|| (pos = strstr(argv[iter], "-?"))
					|| (pos = strstr(argv[iter], "--help"))) {
			}
		}
	}

	free(buf);

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
