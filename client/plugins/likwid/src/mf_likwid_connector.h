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

#ifndef LIKWID_PLUGIN_H_
#define LIKWID_PLUGIN_H_

#include <time.h>

typedef struct Likwid_Plugin_t Likwid_Plugin;

struct Likwid_Plugin_t
{
    int hasPKG;
    int hasDRAM;
    int hasPP0;
    int hasPP1;
    char power_names[4][256];
    float power_values[4];
    char sockets[4][4][128];
    char dram[4][4][128];
    char PP0[4][4][128];
    char PP1[4][4][128];
    int numSockets;
};

/**
 * @brief Gets the current processor model used on the platform.
 *
 * @return processor model
 */
char* get_processor_model(void);

/**
 * @brief Returns 1 if the processor is supported by Likwid
 *
 * @return 1 if the processor is supported by Likwid, 0 otherwise.
 */
int is_processor_supported(void);

/**
 * @brief Convenient method that checks available of DRAM, PP0, and PP1.
 */
void check_processor(
    Likwid_Plugin *likwid,
    char **named_events,
    size_t num_events
);

/**
 * @brief Gets the current power information as measured by Likwid (static).
 */
void get_power_info(Likwid_Plugin *likwid);

/**
 * @brief Gets the current power and energy counters as measured by Likwid
 *        for the given time interval (dynamic). It should be noted that only the
 *        energy at the current time is measured; no power over the given time
 *        interval.
 */
void get_power_data(
    Likwid_Plugin *likwid,
    char **named_events,
    size_t num_events,
    struct timespec profile_interval
);

#endif
