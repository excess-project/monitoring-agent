/** \file http-post.c

 \author Anthony Sulistio, edited by Nico Eichhorn
 \date April 2013
 Copyright 2014 University of Stuttgart
 */
#include <apr-1/apr_queue.h>

#include "http-post.h"
#include "monitoring-excess.h"
#include "monitoring-new.h"

/* GLOBAL variables */
char execID_[ID_SIZE] = ""; /* storing the execution ID -- UUID is 36 chars */
struct curl_slist *headers_ = NULL;
CURL *curl_ = NULL;

apr_queue_t *data_queue;
apr_pool_t *data_pool;
apr_status_t status = APR_SUCCESS;

char addr[100] = "http://localhost:3000/executions/";

int number_to_send = 5;
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

	FILE *fp;
	char line[200];

	int length = strlen(filepath);
	memcpy(filepath + (length - 4), *filename, sizeof(filename));
	fp = fopen(filepath, "r");
	if (!fp) {
		printf("File not found!\n");
		return 0;
	}
	while (fgets(line, 200, fp) != NULL ) {
		char* pos;
		if ((pos = strstr(line, "host: ")))
//			printf("host:\n%s\n", pos + strlen("host: "));
			sprintf(addr, "%s", pos + strlen("host: "));
	}
	addr[strlen(addr) - 1] = '\0'; //remove newline character !!
	fclose(fp);
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
	pthread_t threads[NUM_THREADS];
	int iret[NUM_THREADS];
	apr_initialize();
	apr_pool_create(&data_pool, NULL);
//	int t;

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
	send_data();
//	while (1)
//		;
//	sleep(10e8);
	for (t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL );
	}

}

void *gather(void *arg) {
	int (*p[NUM_THREADS])() = {gather_cpu, gather_mem };
		int *typeT = (int*) arg;
		if (*typeT < NUM_THREADS) {
			if ((*p[*typeT])()) {
				printf("Thread created!");
			}
		}
		pthread_exit(NULL );
	}

	void prepSend(sensor_msg_t *data) {
		char msg[500] = "";
		switch (data->type) {
		case MEM_USAGE:
			sprintf(msg,
					"{\"Timestamp\":\"%lu\",\"type:\":\"mem\",\"mem_used\":\"%d\",\"mem_avail\":\"%d\"}",
					data->mem_time.tv_sec, data->ram_used, data->ram_avail);

			printf("\n\n-> Sending: %s -- len: %d\n", msg, (int) strlen(msg));
			send_monitoring_data(addr, msg);
			break;
		case CPU_USAGE:
			sprintf(msg,
					"{\"Timestamp\":\"%lu\",\"type:\":\"cpu\",\"cpu_load\":\"%f\",\"cpu_avail\":\"%f\",\"t_cpu_waiting_io\":\"%f\"}",
					data->cpu_time.tv_sec, data->cpu_used, data->cpu_avail,
					data->cpu_wa_io);
			send_monitoring_data(addr, msg);
			break;
		default:
			printf("Message to send neither cpu nor mem related!");
			break;
		}
	}

	int send_data() {
		void *ptr;
		while (1) {
			status = apr_queue_pop(data_queue, &ptr);
			if (status == APR_SUCCESS) {
				sensor_msg_t *dPtr = ptr;
				prepSend(dPtr);
			}
		}

		return 1;
	}

	int gather_cpu() {
		fprintf(stderr, "start gather_cpu()\n");
		sensor_msg_t *curPtr; // pointer to message above
		double usage; // value of cpu usage

		while (1) {

			curPtr = apr_palloc(data_pool, sizeof(sensor_msg_t));
			if (curPtr == 0) {
				printf("Failed palloc/n");
			}
			usage = get_cpu_usage();

			curPtr->cpu_used = usage;
			curPtr->cpu_avail = 100.0 - usage;
			curPtr->cpu_time.tv_sec = time(NULL );
			curPtr->cpu_wa_io = usage / 100.0;
			curPtr->type = CPU_USAGE;

//		for (int a = 0; a < 2; a++) {
			status = apr_queue_push(data_queue, curPtr);
			if (status != APR_SUCCESS) {
				printf("Failed queue push!");
			}
//		}

//		if (apr_queue_size(data_queue) > 0) {
//			for (int n = 0; n < apr_queue_size(data_queue); n++) {
//				void *ptr2;
//
//				status = apr_queue_pop(data_queue, &ptr2);
//				if (status != APR_SUCCESS) {
//					printf("Failed queue push!");
//				}
//				sensor_msg_t *dPtr = ptr2;
//
//				printf("Value: %f", dPtr->cpu_used);
//
////					double *usageOut = (double *) ptrO;
////					printf("%f", *usageOut);
//
//			}

		}
//		}
		fprintf(stderr, "gather_cpu ended");
//		exit(EXIT_FAILURE);
		return 0;
	}
	int gather_mem() {
		fprintf(stderr, "start gather_mem()\n");
		sensor_msg_t *curPtr;
		int usage;
		while (1) {
			curPtr = apr_palloc(data_pool, sizeof(sensor_msg_t));
			usage = get_mem_usage();

			curPtr->mem_time.tv_sec = time(NULL );
			curPtr->ram_used = usage;
			curPtr->ram_avail = 100 - usage;
			curPtr->type = MEM_USAGE;

			status = apr_queue_push(data_queue, curPtr);

//			void *ptr = &usage;
//			void **ptr2 = &ptr;
//			apr_queue_trypush(data_queue, ptr2);
		}

		printf("gather_mem ended");
		return 0;
	}

//	void send_dummy_data(char *URL) {
//		char msg[500] = "";
//		int i = 0;
//		sensor_msg_t data;
//
//		printf("\n");
//		for (i = 0; i < END_INDEX; i++) {
//			data = dequeue();
//
//			/* send memory info */
//			sprintf(msg,
//					"{\"Timestamp\":\"%lu\",\"mem_used\":\"%d\",\"mem_avail\":\"%d\"}",
//					data.mem_time.tv_sec, data.ram_used, data.ram_avail);
//			/***
//			 to_send_msg[i].mem_time.tv_sec, to_send_msg[i].ram_used,
//			 to_send_msg[i].ram_avail);
//			 ****/
//
//			printf("\n\n-> Sending: %s -- len: %d\n", msg, (int) strlen(msg));
//			send_monitoring_data(URL, msg);
//
//			/* send CPU info */
//			sprintf(msg,
//					"{\"Timestamp\":\"%lu\",\"cpu_load\":\"%f\",\"cpu_avail\":\"%f\",\"t_cpu_waiting_io\":\"%f\"}",
//					data.cpu_time.tv_sec, data.cpu_used, data.cpu_avail,
//					data.cpu_wa_io);
//			/****
//			 to_send_msg[i].cpu_time.tv_sec, to_send_msg[i].cpu_used,
//			 to_send_msg[i].cpu_avail, to_send_msg[i].cpu_wa_io);
//			 *****/
//
//			printf("\n\n-> Sending: %s -- len: %d\n", msg, (int) strlen(msg));
//			send_monitoring_data(URL, msg);
//		}
//	}

	int getFQDN(char *fqdn) {
		struct addrinfo hints, *info, *p;

		int gai_result;

		char *hostname = (char*) malloc(sizeof(char) * 80);
		gethostname(hostname, sizeof hostname);

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_CANONNAME;

		if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
			exit(1);
		}
		for (p = info; p != NULL ; p = p->ai_next) {
			sprintf(fqdn, "hostname: %s\n", p->ai_canonname);
		}
		freeaddrinfo(info);
		return 1;
	}

	int main(int argc, const char *argv[]) {

		if (!getconf(argv)) {
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

