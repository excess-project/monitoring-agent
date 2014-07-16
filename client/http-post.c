/** \file http-post.c

 \author Anthony Sulistio, edited by Nico Eichhorn
 \date April 2013
 Copyright 2014 University of Stuttgart
 */

#include <sys/stat.h>

#include "http-post.h"
#include "monitoring-excess.h"
#include "monitoring-new.h"

/* GLOBAL variables */
char execID_[ID_SIZE] = ""; /* storing the execution ID -- UUID is 36 chars */
struct curl_slist *headers_ = NULL;
CURL *curl_ = NULL;

//apr_queue_t *data_queue;
//apr_pool_t *data_pool;
apr_status_t status = APR_SUCCESS;

char addr[100] = "http://localhost:3000/executions/";
char *confFile;
struct timespec timeStampFile = { 0, 0 };

pthread_t threads[256];

int t; // switch for running the individual gathering routines

/* ptr - curl output
 stream - data or string to be received */
size_t get_stream_data(void *ptr, size_t size, size_t count, void *stream) {
	size_t total = size * count;

	printf("\n--> Sent Output: %s -- len = %d", (char*) ptr, (int) total);
	memcpy(stream, ptr, total);
	return total;
}

int getconf(const char *argv[]) {
	const char *filename[] = { "conf" };
	char *filepath = strdup(*argv);

	int length = strlen(filepath);
	memcpy(filepath + (length - 4), *filename, sizeof(filename));
	confFile = strdup(filepath);
	return 1;
}

int send_monitoring_data(char *URL, char *data) {
	CURLcode res;
	int result = SEND_SUCCESS;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"send_monitoring_data(): Error - the given url is empty.\n");
		return SEND_FAILED;
	}

	/*****  // Not working well for query the results
	 if (data == NULL || strlen(data) == 0)
	 {
	 fprintf(stderr, "send_monitoring_data(): Error - the monitoring data is empty.\n");
	 return SEND_FAILED;
	 }
	 ******/

	/* init libcurl if not already done */

	if (curl_ == NULL ) {
		init_curl();
	}

	printf("curl -X POST %s -- len: %d\n", URL, (int) strlen(URL));
	printf("Msg = %s -- len: %d\n", data, (int) strlen(data));

	curl_easy_setopt(curl_, CURLOPT_URL, URL);
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, (long ) strlen(data));

	/* get the result or output */
	/*******
	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_stream_data);
	 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
	 ********/

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl_);
	/*printf("\nResult = %d -- ID: %s -- len: %d\n", res, str, (int)strlen(str));*/

	/* Check for errors */
	if (res != CURLE_OK) {
		result = SEND_FAILED;
		fprintf(stderr, "send_monitoring_data() failed: %s\n",
				curl_easy_strerror(res));

	}

	/* in a loop, do a reset instead */
	curl_easy_reset(curl_);
	return result;
}

///* sleep in second */
//void delay_time(time_t delay) {
//	time_t timer0, timer1;
//
//	if (delay <= 0) {
//		return;
//	}
//
//	time(&timer0);
//
//	do {
//		time(&timer1);
//	} while ((timer1 - timer0) < delay);
//}

void init_curl() {
	if (curl_ != NULL ) {
		return;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	curl_ = curl_easy_init();

	headers_ = curl_slist_append(headers_, "Accept: application/json");
	headers_ = curl_slist_append(headers_, "Content-Type: application/json");
	headers_ = curl_slist_append(headers_, "charsets: utf-8");
}

void cleanup_curl() {
	curl_easy_cleanup(curl_);
	curl_slist_free_all(headers_); /* free the header list */
	curl_global_cleanup();
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

void start_gathering(void) {
	printf("Entering start_gathering\n");

	int iret[NUM_THREADS];
	apr_initialize();
	apr_pool_create(&data_pool, NULL);
//	int t;

	if (NUM_THREADS <= 2) {
		fprintf(stderr, "Number of threads to small!");
	}
	apr_queue_create(&data_queue, 100000, data_pool);
	int nums[NUM_THREADS]; // each thread needs its own element of the array
	for (t = 0; t < NUM_THREADS; t++) {
		nums[t] = t;
		iret[t] = pthread_create(&threads[t], NULL, gather, &nums[t]);
		if (iret[t]) {
			printf("ERROR; return code from pthread_create() is %d\n", iret[t]);
			exit(-1);
		}
	}

	struct sigaction sig;
	sig.sa_handler = catcher;
	sig.sa_flags = SA_RESTART;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGTERM, &sig, NULL );
	while (1)
		;

	for (t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL );
	}

}

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
			if ((pos = strstr(line, "papis"))) {
				int where = 0;
				char *p = NULL;
				p = strtok(pos + strlen("papis") + 2, ",");
				while (p) {

					strcpy(papiEvents[where], p);
					// we don't want a end of line character
					if (papiEvents[where][strlen(papiEvents[where]) - 1]
							== '\n')
						papiEvents[where][strlen(papiEvents[where]) - 1] = '\0';
					p = strtok(NULL, ",");
					where++;
				}
				papiNumbers = where;
				//TODO dynamic array for papinames
//				papiNames = malloc(where * sizeof(char*));

			}
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
		plausable(NULL );
	}
	return 1;
}

int checkConf() {
	while (running) {
		readConf(confFile);
		sleep(timingCheck);
	}
	return 1;
}

void *gather(void *arg) {
	int (*p[])() = {gather_cpu, gather_mem, checkConf, send_data, gather_papi };
		int *typeT = (int*) arg;
		if (*typeT < NUM_THREADS) {
			if ((*p[*typeT])()) {
				printf("Thread created!");
			}
		}
		pthread_exit(NULL );
	}

	void prepSend(metric_t *data) {
		char msg[500] = "";
		jidd timeStamp = data->timestamp.tv_sec
				+ (jidd) (data->timestamp.tv_nsec / 10e8);

		sprintf(msg, "{\"Timestamp\":\"%.9Lf\"%s}", timeStamp, data->msg);
		send_monitoring_data(addr, msg);

	}

	int send_data() {

		void *ptr;
		while (running) {
			sleep(timingSend);

			status = apr_queue_pop(data_queue, &ptr);
			if (status == APR_SUCCESS) {
				metric_t *mPtr = ptr;
				prepSend(mPtr);
//				sensor_msg_t *dPtr = ptr;
//				prepSend(dPtr);
			}
		}

		return 1;
	}
	char* toCpuData(sensor_msg_t *ptr) {
		char* returnMsg = malloc(500 * sizeof(char));

		sprintf(returnMsg,
				",\"cpu_used\":\"%f\",\"cpu_avail\":\"%f\",\"t_cpu_waiting_io\":\"%f\"",
				ptr->cpu_used, ptr->cpu_avail, ptr->cpu_wa_io);

		return returnMsg;
	}

	int gather_cpu() {
		fprintf(stderr, "start gather_cpu()\n");
		sensor_msg_t *curPtr; // pointer to message above

		metric_t *resMetric = malloc(sizeof(metric_t));
		resMetric->msg = malloc(sizeof(char) * 500);

		double usage; // value of cpu usage
		int clk_id = CLOCK_REALTIME;

		while (running) {

			curPtr = apr_palloc(data_pool, sizeof(sensor_msg_t));
			if (curPtr == 0) {
				printf("Failed palloc/n");
			}
			clock_gettime(clk_id, &resMetric->timestamp);
			usage = get_cpu_usage();

			curPtr->cpu_used = usage;
			curPtr->cpu_avail = 100.0 - usage;

			curPtr->cpu_wa_io = usage / 100.0;
			curPtr->type = CPU_USAGE;

			strcpy(resMetric->msg, toCpuData(curPtr));

			status = apr_queue_push(data_queue, resMetric);
			if (status != APR_SUCCESS) {
				printf("Failed queue push!");
			}

		}
//		}
		fprintf(stderr, "gather_cpu ended");
//		exit(EXIT_FAILURE);
		return 0;
	}

	char* toMemData(sensor_msg_t *ptr) {
		char *returnMsg = malloc(500 * sizeof(char));

		sprintf(returnMsg, ",\"mem_used\":\"%d\",\"mem_avail\":\"%d\"",
				ptr->ram_used, ptr->ram_avail);

		return returnMsg;
	}
	int gather_mem() {
		fprintf(stderr, "start gather_mem()\n");
		sensor_msg_t *curPtr;
		metric_t *resMetric = malloc(sizeof(metric_t));
		resMetric->msg = malloc(sizeof(char) * 500);

		int usage;
		int clk_id = CLOCK_REALTIME;
		while (running) {
			curPtr = apr_palloc(data_pool, sizeof(sensor_msg_t));
			usage = get_mem_usage();

			clock_gettime(clk_id, &resMetric->timestamp);
			curPtr->ram_used = usage;
			curPtr->ram_avail = 100 - usage;
			curPtr->type = MEM_USAGE;

//			resMetric->msg = toMemData(curPtr);
			strcpy(resMetric->msg, toMemData(curPtr));

			status = apr_queue_push(data_queue, resMetric);
			if (status != APR_SUCCESS) {
				printf("Failed queue push!");
			}

		}

		printf("gather_mem ended");
		return 0;
	}
	int gather_papi() {
		int retval;
		int i;
		int EventSet = PAPI_NULL;

		retval = PAPI_library_init(PAPI_VER_CURRENT);
		if (retval != PAPI_VER_CURRENT && retval > 0) {
			fprintf(stderr, "getPapiValues: PAPI library version mismatch!\n");
			exit(1);
		}

		retval = PAPI_create_eventset(&EventSet);
		if (retval != PAPI_OK) {
			handle_error(retval);
		}

		if (retval < 0) {
			fprintf(stderr, "getPapiValues: Initialization error!\n");
			exit(1);
		}

		for (i = 0; i < MAX_PAPI; i++) {
			if (papiEvents[i][0] == '\0')
				break;
			retval = PAPI_add_named_event(EventSet,
					(char*) (intptr_t) papiEvents[i]);
			if (retval != PAPI_OK) {
				fprintf(stderr,
						"getPapiValues: Failure to add PAPI event '%s'.\n",
						(char*) (intptr_t) papiEvents[i]);
				handle_error(retval);
			}
		}

		retval = PAPI_start(EventSet);
		if (retval != PAPI_OK) {
			handle_error(retval);
		}

		long_long values[MAX_PAPI];

		gatherPapiData(&EventSet, &values);

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

	void plausable() {
		int bound = 10e5;
		for (int iter = 0; iter < NUM_THREADS; iter++) {
			if ((timings[iter] > 0 && timings[iter] != 0)
					&& timings[iter] < bound) {
				printf(
						"\n >>>> wrong value for timing, results become unprecise for values less than %.2e <<<<\n",
						(float) bound);
				exit(1);
			}
		}

	}

	void catcher(int signo) {
		running = 0;
		printf("Signal %d catched\n", signo);
		exit(0);
	}

	int main(int argc, const char *argv[]) {
		running = 1;
		getconf(argv);
		if (!readConf(confFile)) {
			printf("error reading config file!\n");
			exit(-1);
		}

//		printf("%ld\n",sysconf(_SC_CLK_TCK));
		printf("%lf %% \n", get_cpu_usage());
		printf("%d %%\n", get_mem_usage());

		char *timeArr = (char*) malloc(sizeof(char) * 80);
		time_t curTime = time(NULL );
		sprintf(timeArr, "%s", asctime(localtime(&curTime)));

		size_t two = strlen(timeArr);
		timeArr[two - 1] = '\0';

		char str[1000] = ""; /* storing the execution ID -- UUID is 36 chars */
		char msg[1000] = "";

		char *hostname = (char*) malloc(sizeof(char) * 80);

//		gethostname(hostname, sizeof(hostname));
		getFQDN(hostname);
		hostname[strlen(hostname) - 1] = '\0';
		sprintf(msg,
				"{\"Name\":\"Execution on node %s - %s\",\"Description\":\"Testing C gatherer\",\"Other\":\"values\",\"Onemore\":\"please\"}",
				hostname + strlen("hostname: "), timeArr);

		/* init curl libs */
		init_curl();
		strcpy(str, get_execution_id(addr, msg)); /* get the execution ID */
		strcat(addr, str); /* append the ID to the end of the URL or IP address */

		start_gathering();

		cleanup_curl();
		return 0;
	}

