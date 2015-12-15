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

/** @file mf_rapl_connector.h
 *  @brief Interface to the RAPL component of the PAPI library.
 *
 *  This interface declares a means to access measurements of the RAPL
 *  component, which is provided by the PAPI library. Please see the
 *  {@link utils/mf_rapl_client.c mf_rapl_client} for a usage example. Per
 *  default, a developer should first determine if the RAPL component is enabled
 *  (#mf_rapl_is_enabled), then initialize the plug-in via calling
 *  #mf_rapl_init, before doing the actual sampling using #mf_rapl_sample. At
 *  the end, the plug-in should be cleaned by executing #mf_rapl_shutdown.
 *
 *  @author Dennis Hoppe (hopped)
 *
 *  @bug Events are added and removed each time #get_available_events is called.
 */

#ifndef _RAPL_CONNECTOR_H
#define _RAPL_CONNECTOR_H

#include <papi.h> /* PAPI_MAX_PRESET_EVENTS etc */

/** @brief defines the maximum number of available RAPL events.
 *
 * The variable is used to initialize the RAPL data structure.
 */
#define MAX_RAPL_EVENTS 64

/** @brief data structure to store RAPL monitoring data
 */
typedef struct RAPL_Plugin_t RAPL_Plugin;

/** @brief data structure to store RAPL monitoring data
 *
 * The data structure holds the metric names including the correspond
 * measured values. Moreover, the number of events measured is stored.
 */
struct RAPL_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    float values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

/** @brief Checks if the RAPL component is available and enabled
 *
 * This function checks if the RAPL component is compiled within the given
 * PAPI library, and that the RAPL features are enabled.
 *
 * @return 1 if RAPL component is enabled; 0 otherwise.
 */
int mf_rapl_is_enabled();

/** @brief Initializes RAPL plug-in
 *
 * Initialization will be implemented through PAPI library. Moreover, all
 * required variables are initialized and checked for correctness. For instance,
 * in case that a metric name given by user is not supported by PAPI/RAPL, then
 * an error report is given. The return value equals in this case zero.
 *
 * @param data structure that keeps track of RAPL related events and values
 * @param rapl_events user-defined array of metric names to be collected
 * @param num_events equals the length of the array @p rapl_events
 * @return 1 on success; 0 otherwise.
 */
int mf_rapl_init(RAPL_Plugin *data, char **rapl_events, size_t num_events);

/** @brief Performs the actual sampling of metrics
 *
 * This function performs the sampling of RAPL metrics as registered through the
 * function mf_rapl_init.
 *
 * @param data the initialized structure previously passed to mf_rapl_init
 * @return 1 on success; 0 otherwise.
 */
int mf_rapl_sample(RAPL_Plugin *data);

/** @brief Converts the samples data into a JSON object
 *
 * Given RAPL events and corresponding values will be converted into a JSON
 * document, which then can be sent to and processed by the monitoring server.
 *
 * @param data collected metric data
 * @return a string representation (JSON format) of the given @p data
 */
char* mf_rapl_to_json(RAPL_Plugin *data);

/** @brief Shuts down RAPL
 *
 * This method calls internally the PAPI library to shut down gracefully.
 */
void mf_rapl_shutdown();

#endif /* _RAPL_CONNECTOR_H */