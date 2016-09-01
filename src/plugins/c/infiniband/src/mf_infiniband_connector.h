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

#ifndef _MF_INFINIBAND_CONNECTOR_H
#define _MF_INFINIBAND_CONNECTOR_H

#include <papi.h> /* PAPI_MAX_PRESET_EVENTS etc */

/** @brief data structure to store RAPL monitoring data
 */
typedef struct INFINIBAND_Plugin_t INFINIBAND_Plugin;

/** @brief data structure to store INFINIBAND monitoring data
 *
 * The data structure holds the metric names including the correspond
 * measured values. Moreover, the number of events measured is stored.
 */
struct INFINIBAND_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    long long values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

/** @brief Checks if Infiniband component of the PAPI library is enabled
 *
 * This function verifies if the Infiniband component of the PAPI-C library
 * is enabled or disabled. A developer should call this function first, before
 * continuing with calling #mf_infiniband_init.
 *
 * @return 1 if component is enabled; 0 otherwise.
 */
int mf_infiniband_is_enabled();

/** @brief Initializes the Infiniband plug-in
 *
 * This function initializes the Infiniband plug-in when it is enabled for the
 * given infrastructure. The function internally creates a new event set, and
 * tries to add given user-defined events. If an event is not available, an
 * appropriate error message is returned.
 *
 * @param data structure that keeps track of Infiniband related events and values
 * @param infiniband_events user-defined array of metric names to be collected
 * @param num_events equals the length of the array @p infiniband_events
 *
 * @return 1 if all events were valid; 0 otherwise.
 */
int mf_infiniband_init(INFINIBAND_Plugin *data, char **infiniband_events, size_t num_events);

/** @brief Samples the registered Infiniband events
 *
 * This function samples the Infiniband events registered through the
 * #mf_infiniband_init function. The function returns the current metric values
 * at the time of calling it. Then, the counters are reset to zero for the next
 * measurment.
 *
 * @param data that holds the sampled Infiniband values
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_infiniband_sample(INFINIBAND_Plugin *data);

/** @brief Conversion of samples data to a JSON document
 *
 * This function generates a string representation using the JSON format of the
 * given Infiniband data structure. The JSON-like structure is already
 * extended by additional information including a timestamp to be send directly
 * to the monitoring server.
 *
 * @param data that holds the sampled Infiniband values
 *
 * @return a JSON document representing the given Infiniband data
 */
char* mf_infiniband_to_json(INFINIBAND_Plugin *data);

/** @brief Stops sampling Infiniband related events
 *
 * This method calls internally the PAPI library to shut down gracefully.
 */
void mf_infiniband_shutdown();

#endif /* _MF_INFINIBAND_CONNECTOR_H */