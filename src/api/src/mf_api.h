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

/**
 * @brief Defines the external API to the monitoring framework.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#ifndef MF_API_H_
#define MF_API_H_

/**
 * @brief if the experiment is not registered by v1/mf/experiments, 
          create the experiemnt.
          On success return the created experiment_id
 */
char* mf_api_create_experiment(const char* URL, char* wf_id, char* task_id);

/**
 * @brief Initializes the monitoring; has to be called in advance.
          Prepare URLs for getting statistics data and profiles data. 
 */
void mf_api_initialize(const char* URL, char* wf_id, char* exp_id, char* task_id);

/**
 * @brief Query the database in order to retrieve stats metric value collected
 *        during the entire time interval
 */
void mf_api_stats_data_by_metric(char *Metrics_name, char *res);

/**
 * @brief Query the database in order to retrieve stats of multiple metrics
 *        during the entire time interval
 */
void mf_api_stats_metrics(char **Metrics_name, int Metrics_num, char *res);

/**
 * @brief Query the database in order to retrieve stats metric value collected
 *        within the given range: the interval is defined by the two
 *        timestamps start_time and stop_time.
 */
void mf_api_stats_data_by_interval(char *Metrics_name, long double start_time, long double stop_time, char *res);

/**
 * @brief Query the database in order to retrieve stats of multiple metrics
 *        within the given range: the interval is defined by the two
 *        timestamps start_time and stop_time.
 */
void mf_api_stats_metrics_by_interval(char **Metrics_name, int Metrics_num, long double start_time, long double stop_time, char *res);

/**
 * @brief Query the database in order to retrieve all metrics data for a specific experiment
 */
void mf_api_get_profiles_data(char *res);

/**
 * @brief Starts the monitoring of given external function.
 *	return the timestamp of start profiling
 */
long double mf_api_start_profiling(const char *function_name);

/**
 * @brief Sends data formatted in a JSON-like format using key-value pairs.
 */
void mf_api_send(const char* json);

/**
 * @brief Stops the monitoring of the given external function.
 *	return the timestamp of stop profiling
 */
long double mf_api_stop_profiling(const char *function_name);


#endif
