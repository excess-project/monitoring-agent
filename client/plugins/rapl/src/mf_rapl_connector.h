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
#ifndef RAPL_PLUGIN_H_
#define RAPL_PLUGIN_H_

#include <papi.h>
#include <time.h>

#define MAX_RAPL_EVENTS 64

typedef struct RAPL_Plugin_t RAPL_Plugin;


struct RAPL_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    float values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};


/**
 * @brief Gets all available events
 *
 * This function profiles the system for the given time interval (cf. second
 * parameter). Then, the counter values are written to the respective fields
 * of the RAPL_Plugin struct. On success, the return value equals the number
 * of events fetched.
 *
 * @return number of events available
 */
int get_available_events(RAPL_Plugin *rapl, struct timespec profile_interval, char **named_events, size_t num_events, int cpu_model);

/**
 * @brief Gets the RAPL component ID
 *
 * While trying to retrieve the RAPL component ID, a developer can check if the
 * RAPL component is available on the system in question.
 *
 * @return 1 if RAPL component is available; 0 otherwise.
 */
int get_rapl_component_id();

#endif
void mf_rapl_shutdown();
