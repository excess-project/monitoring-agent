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
/**
 * @brief check if config at a specific frequency
 */
int checkConf();

/**
 * @brief start the different threads of the tool
 */
int startThreads();
/**
 * @brief entry function for threads, purpose of each thread is defined here
 */
void *entryThreads(void *arg);
/**
 * @brief gather a metric, use the hook of the plugin
 */
int gatherMetric(int num);
/**
 * @brief pop from queue and send data, hook for sending thread
 */
int startSending();
/**
 *
 */
/**
 * @brief prepare for sending data
 */
int prepSend(metric data);

/**
 * @brief initialize curl to prepare the sending
 */
void init_curl();

/**
 * @brief cleanup curl after sending
 */
void cleanup_curl();

/**
 * @brief send the data to the provided host
 */
int send_monitoring_data(char *URL, char *data);

#endif /* THREAD_HANDLER_H_ */
