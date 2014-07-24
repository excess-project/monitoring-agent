/*
 * excess_main.c
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>

#include "util.h"
#include "thread_handler.h"
#include "excess_main.h"

char *confFile;

long timings[256];

struct timespec timeStampFile = { 0, 0 };

char addr[100] = "http://localhost:3000/executions";

char *pwd;

int readConf(char *confFile) {
	int tempTim = 0;
//	int iter;
	struct stat st;
	char helpAddr[300] = { "" };

	stat(confFile, &st);
	if (st.st_ctim.tv_sec > timeStampFile.tv_sec) {
		if (timeStampFile.tv_sec > 0)
			printf("\nConf file changed, read again!\n");

		FILE *fp;
		char line[200];
		fp = fopen(confFile, "r");
		timeStampFile = st.st_ctim;
		if (!fp) {
			printf("File not found!\n");
			return 0;
		}
		while (fgets(line, 200, fp) != NULL ) {
			char* pos;
			//		printf("%s", line);
			if ((pos = strstr(line, "host: "))) {
				sprintf(helpAddr, "%s", pos + strlen("host: "));
				strncpy(addr, helpAddr, strlen(helpAddr) - 1);
			}
			if ((pos = strstr(line, "timing_"))) {
				int numTim = atoi(pos + strlen("timing_"));
				tempTim = atoi(pos + strlen("timing_") + 2);
				timings[numTim + MIN_THREADS] = tempTim;
				fprintf(stderr, "timing no %d is: %ld \n", numTim + MIN_THREADS,
						timings[numTim + MIN_THREADS]);
			}
			if ((pos = strstr(line, "timingSend")))
				timings[0] = atoi(pos + strlen("timingSend") + 1);
			if ((pos = strstr(line, "timingCheck")))
				timings[1] = atoi(pos + strlen("timingCheck") + 1);

		}

		fclose(fp);

	}
	return 1;
}

int getConf(const char *argv) {
	confFile = malloc(200 * sizeof(char));
	const char *filename = { "/conf" };

	confFile = strdup(argv);
	strcat(confFile, filename);
	fprintf(stderr, "confFile is: %s \n", confFile);

	return 1;

}

int getFQDN(char *fqdn) {
	struct addrinfo hints, *info, *p;

	int gai_result;

	char *hostname = (char*) malloc(sizeof(char) * 80);
	gethostname(hostname, sizeof hostname);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 1) {
		fprintf(stderr, "getaddrinfo: %s,\n using regular hostname",
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

size_t get_stream_data(void *ptr, size_t size, size_t count, void *stream) {
	size_t total = size * count;

	printf("\n--> Sent Output: %s -- len = %d", (char*) ptr, (int) total);
	memcpy(stream, ptr, total);
	return total;
}

char* get_execution_id(char *URL, char *msg) {
	CURLcode res;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"get_execution_id(): Error - the given url is empty.\n");
		return NULL ;
	}

	if (msg == NULL || strlen(msg) == 0) {
		fprintf(stderr,
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
		return NULL ;

	}

	curl_easy_reset(curl_);
	return execID_;
}

char* cutPwd(char *pwd) {
	char* help = malloc(200 * sizeof(char));
	char *lastslash = strrchr(pwd, '/');
	int ptr = lastslash - pwd;

	strncpy(help, pwd, ptr);

	return help;
}

int main(int argc, const char* argv[]) {
	char *buf[200];
	readlink("/proc/self/exe", *buf, 200); // obtain full path of executable

	char *pos;
	if (argc > 1) {
		for (int iter = 0; iter < argc; iter++) {
			fprintf(stderr, "arg #%d is: %s\n", iter, argv[iter]);
			if ((pos = strstr(argv[iter], "-id="))) {
				strcpy(execID_, pos + strlen("-id="));
			}
			if ((pos = strstr(argv[iter], "-h"))
					|| (pos = strstr(argv[iter], "-?"))
					|| (pos = strstr(argv[iter], "--help"))) {
			}
		}
	}
	pwd = malloc(200 * sizeof(char));
	strcpy(pwd, *buf);
	pwd = cutPwd(pwd);

	getConf(pwd);
	readConf(confFile);

	char *timeArr = (char*) malloc(sizeof(char) * 80);
	time_t curTime = time(NULL );
	sprintf(timeArr, "%s", asctime(localtime(&curTime)));

	size_t two = strlen(timeArr);
	timeArr[two - 1] = '\0';

	char str[1000] = ""; /* storing the execution ID -- UUID is 36 chars */
	char msg[1000] = "";

	char *hostname = (char*) malloc(sizeof(char) * 80);

	getFQDN(hostname);
	hostname[strlen(hostname) - 1] = '\0';
	sprintf(msg,
			"{\"Name\":\"Execution on node %s - %s\",\"Description\":\"Testing C gatherer\",\"Other\":\"values\",\"Onemore\":\"please\"}",
			hostname + strlen("hostname: "), timeArr);

	/* init curl libs */
	init_curl();
	strcpy(str, get_execution_id(addr, msg)); /* get the execution ID */
	strcat(addr, str);

	if (!startThreads())
		fprintf(stderr, "Couldn't start the threads!\n");

	exit(0);
}
