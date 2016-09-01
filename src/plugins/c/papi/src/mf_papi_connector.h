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

/** @file mf_papi_connector.h
 *  @brief Interface to the PAPI component of the PAPI-C library.
 *
 *  This interface declares a means to access measurements of the PAPI
 *  component, which is provided by the PAPI-C library. Please see the
 *  {@link utils/mf_papi_client.c mf_papi_client} for a usage example. Per
 *  default, a developer should first initialize the plug-in via calling
 *  #mf_papi_init, before doing the actual sampling using #mf_papi_sample. At
 *  the end, the plug-in should be gracefully terminated by executing
 *  #mf_papi_shutdown.
 *
 *  @author Dennis Hoppe (hopped)
 */

#ifndef _PAPI_CONNECTOR_H
#define _PAPI_CONNECTOR_H

#include <papi.h> /* PAPI_MAX_PRESET_EVENTS etc */

/** @brief data structure to store PAPI monitoring data
 */
typedef struct PAPI_Plugin_t PAPI_Plugin;

/** @brief data structure to store PAPI monitoring data
 *
 * The data structure holds the metric names including the correspond
 * measured values. Moreover, the number of events measured is stored.
 */
struct PAPI_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    long long values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

/** @brief Initializes the PAPI plug-in
 *
 * This function loads the underlying PAPI-C library, so that in subsequent
 * steps data can be sampled. In contrast to other initialization functions,
 * this function has a fourth parameter: @p num_cores. It is required to set
 * the number of cores to be measured. If the number of given cores exceeds
 * the amount of available CPU cores, then the physical maximum is taken.
 *
 * Internally, a suitable event set is created, events are bind to CPU cores,
 * and the given user-defined events are registered.
 *
 * @param data structure that holds sampled PAPI events
 * @param papi_events user-defined array of metrics to be sampled
 * @param num_events equals the length of @p papi_events
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_papi_init(PAPI_Plugin **data, char **papi_events, size_t num_events, size_t num_cores);

/** @brief Samples given PAPI events
 *
 * This function samples given PAPI events provided through the #mf_papi_init
 * function. After sampling, the values are stored in the data structure passed
 * to this function, and the corresponding counters are reset to zero. Thus,
 * each call to #mf_papi_sample returns counter values relative to the last
 * execution of #mf_papi_sample.
 *
 * @param data structure that holds sampled PAPI events
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_papi_sample(PAPI_Plugin **data);

/** @brief String representation of sampled PAPI events
 *
 * This functions returns a JSON representation of the PAPI events sampled. The
 * output is compatible with the EXCESS monitoring server, and thus includes
 * additional information such as timestamp of measurement. Please note that
 * a single timestamp for all events sampled by calling #mf_papi_sample.
 *
 * @param data structure that holds sample PAPI events including their values
 *
 * @return JSON document representing the given PAPI data structure
 */
char* mf_papi_to_json(PAPI_Plugin **data);

/** @brief Stops sampling PAPI events
 *
 * This methods shuts down gracefully PAPI sampling.
 */
void mf_papi_shutdown();

#endif /* _PAPI_CONNECTOR_H */