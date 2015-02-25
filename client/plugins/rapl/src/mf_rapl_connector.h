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
    char events[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
    float *values;
    int data_types[MAX_RAPL_EVENTS];
    int num_events;
};

int get_available_events(RAPL_Plugin *rapl, struct timespec profile_interval);

int get_rapl_component_id();

#endif
