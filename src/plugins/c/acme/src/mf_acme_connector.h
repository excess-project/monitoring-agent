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

/** @file mf_acme_connector.h
 *  @brief Interface to the ACME-IIO component based on libiio.
 *
 *  This interface declares a means to access measurements of ACME
 *  board, which is provided by the libiio library. Please see the
 *  {@link utils/mf_acme_client.c mf_acme_client} for a usage example. Per
 *  default, a developer should first determine if the ACME board is accessible 
 *  through network with hostname "baylibre-acme.local"
 *  then initialize the plug-in via calling #mf_acme_init, before doing 
 *  the actual sampling using #mf_acme_sample. At
 *  the end, the plug-in should be cleaned by executing #mf_acme_shutdown.
 *
 *  @author Fangli Pi
 */

#ifndef _ACME_CONNECTOR_H
#define _ACME_CONNECTOR_H


/** @brief defines the maximum number of available ACME events.
 *
 * The variable is used to initialize the ACME data structure.
 */
#define MAX_ACME_EVENTS 64

/** @brief data structure to store ACME monitoring data
 */
typedef struct ACME_Plugin_t ACME_Plugin;

/** @brief data structure to store ACME monitoring data
 *
 * The data structure holds the metric names including the correspond
 * measured values. Moreover, the number of events measured is stored.
 */
struct ACME_Plugin_t
{
    char *events[MAX_ACME_EVENTS];
    double values[MAX_ACME_EVENTS];
    int num_events;
};

/** @brief Checks if the ACME component is available through network
 * if ACME component is available collects events units and send them to mf_server
 *
 * @return 1 if ACME component is enabled; 0 otherwise.
 */
int mf_acme_is_enabled();


/** @brief Initializes ACME devices and prepares the EventsSet for data collection
 *
 * Initialization is implemented through libiio library. Moreover, all
 * required variables are initialized and checked for correctness. For instance,
 * in case that a metric name given by user is not supported by ACME device, then
 * an error report is given. The return value equals in this case zero.
 *
 * @param data structure that keeps track of ACME related events and values
 * @param acme_events user-defined array of metric names to be collected
 * @param num_events equals the length of the array @p acme_events
 * @return 1 on success; 0 otherwise.
 */
int mf_acme_init(ACME_Plugin *data, char **acme_events, size_t num_events);


/** @brief Initialize the units for all possible events */
void mf_acme_unit_init();


/** @brief Performs the actual sampling of metrics
 *
 * This function gets all metrics send through network and filters the events
 * as specified by users.
 *
 * @param data the initialized structure previously passed to mf_acme_init
 * @return 1 on success; 0 otherwise.
 */
int mf_acme_sample(ACME_Plugin *data);


/** @brief Converts the samples data into a JSON object
 *
 * Given acme events and corresponding values will be converted into a JSON
 * document, which then can be sent to and processed by the monitoring server.
 *
 * @param data collected metric data
 * @return a string representation (JSON format) of the given @p data
 */
char* mf_acme_to_json(ACME_Plugin *data);


/** @brief Stops receiving the measurements
 *
 * This method calls internally the libiio library to shut down gracefully.
 */
void mf_acme_shutdown();

#endif /* _ACME_CONNECTOR_H */