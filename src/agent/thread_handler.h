/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef THREAD_HANDLER_H_
#define THREAD_HANDLER_H_

#include <excess_concurrent_queue.h>

#include "mf_types.h"
#include "util.h"

#define MIN_THREADS 2 //minimum number of thread required for sending and config
#define SEND_SUCCESS 1
#define SEND_FAILED  0

EXCESS_concurrent_queue_t data_queue;
/**
 * @brief Check for changes in the configuration file at a given time interval
 */
int checkConf();

/**
 * @brief Starts all threads registered (e.g. for sending data to the server)
 */
int startThreads();

/**
 * @brief Hook function defined individually for each plug-in (i.e. thread)
 */
void *entryThreads(void *arg);

/**
 * @brief Gathering the registered metrics using the hook function of a plug-in
 */
int gatherMetric(int num);

/**
 * @brief Pops new data from the stack in order to send it to the server
 */
int startSending();

/**
 * @brief Convenience method to prepare the data before sending.
 */
int prepSend(metric data);

#endif /* THREAD_HANDLER_H_ */
