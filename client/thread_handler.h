/*
 * thread_handler.h
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#ifndef THREAD_HANDLER_H_
#define THREAD_HANDLER_H_

#include <apr_queue.h>
#include <curl/curl.h>

#include "util.h"

#define MIN_THREADS 2 //minimum number of thread required for sending and config
#define SEND_SUCCESS 1
#define SEND_FAILED  0
#define ID_SIZE 50

extern char execID_[ID_SIZE];
extern struct curl_slist *headers_;
extern CURL *curl_;

struct apr_queue_t *data_queue;
struct apr_pool_t *data_pool;

int checkConf();

int startThreads();

void *entryThreads(void *arg);

int gatherMetric(int num);

int startSending();

int checkConf();

int prepSend(metric data);

void init_curl();

void cleanup_curl();

#endif /* THREAD_HANDLER_H_ */
