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

char *confFile;

long timings[256];
/**
 * @brief timestamp for config file
 */
struct timespec timeStampFile = { 0, 0 };

char addr[100] = "http://localhost:3000/executions";

char *pwd;
struct tm *time_info;
FILE *logFile;
char name[300];

int readConf(const char *confFile) {

//	int iter;
	struct stat st;

	stat(confFile, &st);
	if (st.st_ctim.tv_sec > timeStampFile.tv_sec) {
		if (timeStampFile.tv_sec > 0) {
			fprintf(stderr, "\nConf file changed, read again!\n");
			fprintf(logFile, "\nConf file changed, read again!\n");
		}

		FILE *fp;
		char line[200];
		fp = fopen(confFile, "r");
		timeStampFile = st.st_ctim;
		if (!fp) {
			fprintf(stderr, "File not found!\n%s\n using default values\n",
					confFile);
			fprintf(logFile, "File not found!\n%s\n using default values\n",
					confFile);
			for (int i = 2; i < 256; i++)
				timings[i] = 1000000000;
			timings[0] = 0;
			timings[1] = 30;
			return 1;
		}
		while (fgets(line, 200, fp) != NULL ) {
			char *pos;
			if ((pos = strstr(line, "#"))) {
				continue;
			}
			if ((pos = strstr(line, "host: "))) {
				char helpAddr[300] = { "" };
				sprintf(helpAddr, "%s", pos + strlen("host: "));
				strncpy(addr, helpAddr, strlen(helpAddr) - 1);
				continue;
			}
			if ((pos = strstr(line, "timing_"))) {
				int numTim = atoi(pos + strlen("timing_"));
				int tempTim = atoi(pos + strlen("timing_") + 2);
				timings[numTim + MIN_THREADS] = tempTim;
				fprintf(stderr, "timing no %d is: %ld \n", numTim + MIN_THREADS,
						timings[numTim + MIN_THREADS]);
				fprintf(logFile, "timing no %d is: %ld \n",
						numTim + MIN_THREADS, timings[numTim + MIN_THREADS]);
				continue;
			}
			if ((pos = strstr(line, "timingSend"))) {
				timings[0] = atoi(pos + strlen("timingSend") + 1);
				continue;
			}
			if ((pos = strstr(line, "timingCheck"))) {
				timings[1] = atoi(pos + strlen("timingCheck") + 1);
				continue;
			}

		}

		fclose(fp);

	}
	return 1;
}

int getConf(const char *argv) {
//	confFile = malloc(200 * sizeof(char));
	const char *filename = { "/conf" };

	char *tmpChar = malloc(300 * sizeof(char));
	memset(tmpChar, '\0', 300 * sizeof(char));
	memcpy(tmpChar, argv, strlen(argv) * sizeof(char));
//	confFile = strdup(argv);
	strcat(tmpChar, filename);
	confFile = strdup(tmpChar);
	free(tmpChar);
	fprintf(stderr, "confFile is: %s \n", confFile);
	fprintf(logFile, "confFile is: %s \n", confFile);

	return 1;

}
/**
 * @brief get fully-qualified domain name
 */
int getFQDN(char *fqdn) {
	struct addrinfo hints, *info, *p;

	int gai_result;

	char *hostname = malloc(sizeof(char) * 80);
	gethostname(hostname, sizeof hostname);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 1) {
		fprintf(stderr, "getaddrinfo: %s,\n using regular hostname\n",
				gai_strerror(gai_result));
		fprintf(logFile, "getaddrinfo: %s,\n using regular hostname\n",
				gai_strerror(gai_result));
		FILE *tmp = NULL;
		if ((tmp = popen("hostname", "r")) == NULL ) {
			perror("popen");
			return -1;
		}
		char line[200];
		while (fgets(line, 200, tmp) != NULL )

			sprintf(fqdn, "hostname: %s", line);
		return 1;
	}
	for (p = info; p != NULL ; p = p->ai_next) {
		sprintf(fqdn, "hostname: %s\n", p->ai_canonname);
	}

	if (info->ai_canonname)
		freeaddrinfo(info);

	return 1;
}
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

int prepare() {

	getConf(pwd);
	readConf(confFile);

	char timeArr[80];

	strftime(timeArr, 80, "%c", time_info);

	char str[1000] = ""; /* storing the execution ID -- UUID is 36 chars */
	char msg[1000] = "";

	char *hostname = (char*) malloc(sizeof(char) * 80);

	getFQDN(hostname);
	hostname[strlen(hostname) - 1] = '\0';
	sprintf(msg,
			"{\"Name\":\"Execution on node %s - %s\",\"Description\":\"Testing C gatherer\",\"Other\":\"values\",\"Onemore\":\"please\"}",
			hostname + strlen("hostname: "), timeArr);

	free(hostname);

	/* init curl libs */
	init_curl();
	strcpy(str, get_execution_id(addr, msg)); /* get the execution ID */
	strcat(addr, str);
	return 1;
}

int createLogFile() {

	time_t curTime;
	time(&curTime);
	time_info = localtime(&curTime);
	char logFileName[300] = { '\0' };
	char timeForFile[50];
	strftime(timeForFile, 50, "%F-%T", time_info);
	sprintf(logFileName, "%s/log/log-%s", pwd, timeForFile);
	fprintf(stderr, "using logfile: %s\n", logFileName);

	logFile = fopen(logFileName, "w");
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

	confFile = malloc(300 * sizeof(char));
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
			if ((pos = strstr(argv[iter], " -hostname="))) {
				strcpy(addr, pos + strlen("-hostname="));
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
	exit(EXIT_SUCCESS);
}
