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

#define SEND_SUCCESS 1
#define SEND_FAILED  0
#define ID_SIZE 50

#define NUM_THREADS 2

//#define QUEUE
//#define QUEUE_SEND_SIZE 100000


/* function prototypes for public use */
void init_curl();
void cleanup_curl();
char* get_execution_id(char *URL, char *message);
int send_monitoring_data(char *URL, char *data);
void delay_time(time_t second); /* sleep in second */

int getconf(const char *argv[]);
void *gather(void *arg);

int gather_cpu();
int gather_mem();

int send_data();


/* function prototypes for private use 
 size_t get_stream_data(void *ptr, size_t size, size_t count, void *stream);
 */
void init_monitoring_data(void);

#endif

