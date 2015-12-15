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

/** @brief [brief description]
 *
 * @details [long description]
 *
 * @param data [description]
 * @param papi_events [description]
 * @param num_events [description]
 *
 * @return [description]
 */
int mf_infiniband_init(
    INFINIBAND_Plugin **data,
    char **infiniband_events,
    size_t num_events,
    size_t num_cores
);

/** @brief [brief description]
 *
 * @details [long description]
 *
 * @param data [description]
 *
 * @return [description]
 */
int mf_infiniband_sample(INFINIBAND_Plugin **data);

/** @brief [brief description]
 *
 * @details [long description]
 *
 * @param data [description]
 *
 * @return [description]
 */
char* mf_infiniband_to_json(INFINIBAND_Plugin **data);

/** @brief [brief description]
 *
 * @details [long description]
 */
void mf_infiniband_shutdown();

#endif /* _MF_INFINIBAND_CONNECTOR_H */