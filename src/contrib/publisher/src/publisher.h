/*
 * Copyright (C) 2014-2015 University of Stuttgart
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

/**
 * @brief Publish a message by posting it to user-defined URL
 *
 * The @<Publisher@> is responsible for sending data to user-defined URLs. The
 * current implementation uses cURL to publish string messages. The monitoring
 * framework uses during its prototyping stage the function @<publish_json@> in
 * order to send performance and/or energy counters to the server via JSON.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#define SEND_SUCCESS 1
#define SEND_FAILED  0
#define ID_SIZE 64
#define MAX_WAIT_MSECS 1*1000  /* multi_curl Wait max. 1 seconds */

#include "mf_types.h"

extern char execution_id[ID_SIZE];

/**
 * @brief Creates a new experiment ID based on the given username and message.
 *
 */
char* create_experiment_id(const char* URL, char* message);

/**
 * @brief Queries a server by the given query, and writes the response into
 * received data.
 *
 */
int query(const char* query, char* received_data);

/**
 * @brief Sends the data defined in message to the given URL via cURL.
 *
 * @return 1 if successful; 0 otherwise
 */
int publish_json(const char *URL, char *message);

/**
 * @brief Sends the plugin metrics units to the given URL via cURL.
 *
 * @return 1 if successful; 0 otherwise
 */
int publish_unit(metric_units *units);

/**
 * @brief Check if the units file of a plugin exists.
 * 
 * @return -1 if error; 0 if file not exists; 1 if successful
 */
int unit_file_check(const char *plugin_name);

/**
 * @brief Frees the cURL headers and global variables.
 *
 * Note: This method needs to be private in order to hide cURL.
 */

void shutdown_curl();

/**
 * @brief Perform the non-blocking curl_multi_perform to send metrics data
 *
 * @return a void pointer to the multi-handles 
 */
void* non_block_publish(const char *URL, char *message);

/**
 * @brief Free the finished multi-handles and handle in the multi-handle.
 *
 */
int curl_handle_clean(void *curl_ptr);

#endif
