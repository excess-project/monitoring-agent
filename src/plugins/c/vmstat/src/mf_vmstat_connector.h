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

/** @file mf_vmstat_connector.h
 *  @brief Interface to /proc/vmstat provided by the underlying operating system.
 *
 *  This interface declares a means to access data provided by /proc/vmstat.
 *  Please see the {@link utils/mf_vmstat_client.c mf_vmstat_client} for a
 *  usage example. Per default, a developer should first determine if the
 *  utility is installed (#mf_vmstat_is_enabled), then initialize the plug-in
 *  via calling #mf_vmstat_init, before doing the actual sampling using
 *  #mf_vmstat_sample.
 *
 *  @author Dennis Hoppe (hopped)
 */

#ifndef _MF_VMSTAT_CONNECTOR_H
#define _MF_VMSTAT_CONNECTOR_H

//#include <stddef.h>

/** @brief data structure to store vmstat monitoring data
 */
typedef struct VMSTAT_Plugin_t VMSTAT_Plugin;

/** @brief data structure to store /proc/vmstat monitoring results
 *
 * The data structure holds the metric names including the correspond
 * measured values. Metric names supported by this plug-in are documented
 * in more detail in the README file. Basically, 'cpu'
 */
struct VMSTAT_Plugin_t
{
    char *events[128];
    long long values[128];
    int num_events;
};

/** @brief Checks if vmstat is installed and usable
 *
 * This function checks if vmstat is installed and if values can be read.
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_vmstat_is_enabled();

/** @brief Initializes the VMSTAT plug-in
 *
 * This function checks if /proc/vmstat is available and accessible. Moreover,
 * user-given events are verified against available events.
 *
 * @param vmstat data structure that holds VMSTAT monitoring data
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_vmstat_init(VMSTAT_Plugin *data, char **vmstat_events, size_t num_events);

/** @brief Sample /proc/vmstat based on given events
 *
 * This function samples /proc/vmstat, and stores current metric values for the
 * events provided through #mf_vmstat_init in the data structure.
 *
 * @param data structure holding the current monitoring values
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_vmstat_sample(VMSTAT_Plugin *data);

/** @brief Transform internal data structure to string
 *
 * This function transforms the given data aggregated through the internal data
 * structure to a JSON-like string. The resulting string is then directly
 * append-able to the message sent to the monitoring server.
 *
 * @param vmstat structure holding the current monitoring values
 *
 * @return JSON-like representation of @p data
 */
char* mf_vmstat_to_json(VMSTAT_Plugin *data);

#endif /* _MF_VMSTAT_CONNECTOR_H */
