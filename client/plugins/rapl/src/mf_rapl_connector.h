/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/** @file mf_rapl_connector.h
 *  @brief Interface to the RAPL component of the PAPI library.
 *
 *  This interfaces declares a measn to access measurements of the RAPL
 *  component, which is provided by the PAPI library.
 *
 *  @author Dennis Hoppe (hopped)
 *
 *  @bug Events are added and removed each time get_available_events is called.
 */

#ifndef _RAPL_PLUGIN_H
#define _RAPL_PLUGIN_H

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

int mf_rapl_init(RAPL_Plugin *data, char **rapl_events, size_t num_events);
int mf_rapl_sample(RAPL_Plugin *data);
char* mf_rapl_to_json(RAPL_Plugin *data);

/** @brief Gets all available events
 *
 * This function profiles the system for the given time interval (cf. parameter
 * @p timespec). Then, the counter values are written to the respective fields
 * of the RAPL_Plugin representation. On success, the return value equals the
 * number of events fetched.
 *
 * @param rapl RAPL data structure
 * @param timespec interval used for measuring
 * @param named_events array of metric names to be measured
 * @param num_events size of the array named_events
 * @param cpu_model the given CPU model, i.e., 15 equals Haswell
 *
 * @return number of events available.
 */
 /*
int mf_rapl_get_available_events(
    RAPL_Plugin *rapl,
    char **named_events,
    size_t num_events,
    int cpu_model
);
*/

/** @brief Checks if the RAPL component is available and enabled
 *
 * This function checks if the RAPL component is compiled within the given
 * PAPI library, and that the RAPL features are enabled.
 *
 * @return 1 if RAPL component is enabled; 0 otherwise.
 */
int mf_rapl_is_enabled();

/** @brief [brief description]
 *
 * This function returns the detected CPU model. The CPU model is encoded via
 * a corresponding integer as follows: 14 (Ivy Bridge), 15 (Haswell).
 *
 * @return CPU model id, e.g. 15 (= Haswell)
 */
//int mf_rapl_get_cpu_model();

/** @brief Shuts down RAPL
 *
 * This method calls internally the PAPI library to shut down gracefully.
 */
void mf_rapl_shutdown();

#endif /* _RAPL_PLUGIN_H */