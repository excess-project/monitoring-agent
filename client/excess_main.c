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

#include "util.h"
#include "thread_handler.h"
#include "excess_main.h"

char *confFile;

struct timespec timeStampFile = { 0, 0 };

char addr[100] = "http://localhost:3000/executions";

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
				timings[numTim] = tempTim;
				fprintf(stderr, "timing no %d is: %ld \n", numTim,
						timings[numTim]);
			}
			if ((pos = strstr(line, "timingSend")))
				timingSend = atoi(pos + strlen("timingSend") + 1);
			if ((pos = strstr(line, "timingCheck")))
				timingCheck = atoi(pos + strlen("timingCheck") + 1);

			if ((pos = strstr(line, "papiTiming"))) {
				timings[NUM_THREADS - 1] = atoi(pos + strlen("papiTiming") + 1);
			}
			if ((pos = strstr(line, "noOfThreads"))) {
				NUM_THREADS = atoi(pos + strlen("noOfThreads") + 1);
//				timings = realloc(timings, sizeof(long) * NUM_THREADS);
//				threads = realloc(threads, sizeof(pthread_t) * NUM_THREADS);
			}

		}

		fclose(fp);

	}
	return 1;
}

int getConf(const char *argv[]) {
	const char *filename[] = { "conf" };
	char *filepath = strdup(*argv);

	int length = strlen(filepath);
	memcpy(filepath + (length - 4), *filename, sizeof(filename));
	confFile = strdup(filepath);
	return 1;

}

int main(int argc, const char* argv[]) {

	getConf(argv);
	readConf(confFile);

	if (!startThreads())
		fprintf(stderr, "Couldn't start the threads!\n");
	//get conf, read conf, start main thread

//start_threads

//	quit

	exit(0);
}
