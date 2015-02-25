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

#ifndef _MF_PAPI_CONNECTOR_H_
#define _MF_PAPI_CONNECTOR_H_

#include <papi.h>
#include <time.h>

typedef struct PAPI_Plugin_t PAPI_Plugin;

struct PAPI_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    long long values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

/**
 * @brief Initializes the PAPI component
 *
 * Initializes the PAPI component with the list of given events
 * (named_events), the size of the list, and the request number of CPU cores
 * that events should be registered to. If the number of cores specified is
 * greater than the available amount of CPU cores, the events will be
 * registered to all available CPU Cores.
 *
 */
void mf_papi_init(
    char **named_events,
    size_t num_events,
    int requested_num_cores
);

/**
 * @brief Profiles the system for the given time interval
 *
 * Values will be collected for the given time interval for each named event
 * that was registered with the mf_papi_init method.
 */
void mf_papi_profile(struct timespec profile_interval);

/**
 * @brief Reads the counters measured during the profiling phase
 *
 * Writes both the counter names and their respective value to the PAPI_Plugin
 * struct for further analysis.
 */
void mf_papi_read(PAPI_Plugin *papi, char **events);

/**
 * @brief Shuts down the PAPI component
 */
void mf_papi_shutdown();

#endif