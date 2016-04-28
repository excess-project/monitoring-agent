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

/** @file mf_meminfo_connector.h
 *  @brief Interface to /proc/meminfo provided by the underlying operating system
 *
 *  This interface declares a means to access data provided by /proc/meminfo.
 *  Please see the {@link utils/mf_meminfo_client.c mf_meminfo_client} for a
 *  usage example. Per default, a developer should first determine if the
 *  utility is installed (#mf_meminfo_is_enabled), then initialize the plug-in
 *  via calling #mf_meminfo_init, before doing the actual sampling using
 *  #mf_meminfo_sample.
 *
 *  @author Dennis Hoppe (hopped)
 */

#ifndef _MF_MEMINFO_CONNECTOR_H
#define _MF_MEMINFO_CONNECTOR_H

#include "mf_types.h"
//#include <stddef.h>

/** @brief data structure to store RAPL monitoring data
 */
typedef struct MEMINFO_Plugin_t MEMINFO_Plugin;

/** @brief data structure to store /proc/vmstat monitoring results
 *
 * The data structure holds the metric names including the correspond
 * measured values. Metric names supported by this plug-in are documented
 * in more detail in the README file. Basically, 'cpu'
 */
struct MEMINFO_Plugin_t
{
    char *events[128];
    long long values[128];
    int num_events;
};

/** @brief Checks if /proc/meminfo is readable
 *
 * This function checks if /proc/meminfo is installed and if values can be read.
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_meminfo_is_enabled();

/** @brief Initializes the /proc/meminfo plug-in
 *
 * This function checks if /proc/meminfo is available and accessible. Moreover,
 * user-given events are verified against available events.
 *
 * @param meminfo data structure that holds VMSTAT monitoring data
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_meminfo_init(MEMINFO_Plugin *data, char **meminfo_events, size_t num_events);

int mf_meminfo_unit_init(metric_units *unit);
/** @brief Sample /proc/meminfo based on given events
 *
 * This function samples /proc/meminfo, and stores current metric values for the
 * events provided through #mf_meminfo_init in the data structure.
 *
 * @param data structure holding the current monitoring values
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_meminfo_sample(MEMINFO_Plugin *data);

/** @brief Transform internal data structure to string
 *
 * This function transforms the given data aggregated through the internal data
 * structure to a JSON-like string. The resulting string is then directly
 * append-able to the message sent to the monitoring server.
 *
 * @param meminfo structure holding the current monitoring values
 *
 * @return JSON-like representation of @p data
 */
const char* mf_meminfo_to_json(MEMINFO_Plugin *data);

/** @brief Stops measuring /proc/meminfo events
 *
 * This method stops sampling /proc/meminfo.
 */
void mf_meminfo_shutdown();

#endif /* _MF_MEMINFO_CONNECTOR_H */
