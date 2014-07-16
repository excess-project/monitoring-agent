/** \file http-post.h
 This file contains several function prototypes for sending the monitoring data
 to the server using libcurl libraries.

 \author Anthony Sulistio
 \date April 2013
 Copyright 2014 University of Stuttgart
 */

#ifndef _HTTP_POST_H
#define _HTTP_POST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <curl/curl.h>

#include <pthread.h>
#include <netdb.h>
#include <signal.h>

#include <apr-1/apr_queue.h>

#define SEND_SUCCESS 1
#define SEND_FAILED  0
#define ID_SIZE 50

//#define NUM_THREADS 5 // number of threads, minimum is 2,
//#define MAX_PAPI 256
int NUM_THREADS;
long timings[256]; // 1 s = 1,000 ms= 1,000,000 µs = 1,000,000,000 ns
char papiEvents[256][20];

int papiNumbers;

struct apr_queue_t *data_queue;
struct apr_pool_t *data_pool;
//struct apr_status_t status = APR_SUCCESS;

//char **papiNames;

int timingSend, timingCheck; // for now in seconds

int running; // value is 1 so program is running

//#define QUEUE
//#define QUEUE_SEND_SIZE 100000

/* function prototypes for public use */
void init_curl();
void cleanup_curl();
char* get_execution_id(char *URL, char *message);
int send_monitoring_data(char *URL, char *data);
void delay_time(time_t second); /* sleep in second */

int getconf(const char *argv[]);
int getFQDN(char *fqdn);
void *gather(void *arg);

int gather_cpu();
int gather_mem();
int gather_papi();

int send_data();

void plausable();

void catcher(int signo);

/* function prototypes for private use 
 size_t get_stream_data(void *ptr, size_t size, size_t count, void *stream);
 */
void init_monitoring_data(void);

#endif

