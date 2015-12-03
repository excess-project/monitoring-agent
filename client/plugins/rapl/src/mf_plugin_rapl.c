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

#include <mf_parser.h>
#include <stdlib.h>

#include "mf_rapl_connector.h"
#include "plugin_manager.h"
#include "util.h"

struct timespec profile_time = { 0, 0 };
mfp_data *conf_data;
int cpu_model;
int is_available = 0;

char* to_JSON(RAPL_Plugin *rapl)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"energy\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < rapl->num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%.4f", rapl->events[i], rapl->values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    return json;
}

static metric
mf_plugin_rapl_hook()
{
    if (running && (is_available == 1)) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);
        RAPL_Plugin *rapl = malloc(sizeof(RAPL_Plugin));
        get_available_events(rapl, profile_time,conf_data->keys, conf_data->size, cpu_model);
        strcpy(resMetric->msg, to_JSON(rapl));
        free(rapl);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int
init_mf_plugin_rapl(PluginManager *pm)
{
    is_available = get_rapl_component_id();

    /*
     * processor info and feature bits
     */
    unsigned eax, ebx, ecx, edx;
    eax = 1;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    /*
     * if (cpu_model == 14) {
     *   then we have detected either node01 or node02
     * }
     *
     * if (cpu_model ==15) {
     *   then it is EXCESS cluster node node03
     * }
     */
    cpu_model = (eax >> 4) & 0xF;

    PluginManager_register_hook(pm, "mf_plugin_rapl", mf_plugin_rapl_hook);
    conf_data =  malloc(sizeof(mfp_data));

    mfp_get_data_filtered_by_value("mf_plugin_rapl", conf_data, "on");

    char* value = mfp_get_value("timings", "mf_plugin_rapl");
    long timing = atoi(value);
    timing = timing / 2.0;

    if (timing >= 10e8) {
        profile_time.tv_sec = timing / 10e8;
        profile_time.tv_nsec = timing % (long) 10e8;
    } else {
        profile_time.tv_sec = 0;
        profile_time.tv_nsec = timing;
    }

    return 1;
}
