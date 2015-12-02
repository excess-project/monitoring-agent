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
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "excess_main.h"
#include "mf_debug.h"
#include "mf_infiniband_connector.h"
#include "plugin_manager.h"
#include "util.h"

struct timespec profile_time = { 0, 0 };
mfp_data *conf_data;


char*
to_JSON(INFINIBAND_Plugin *infiniband)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"performance\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < infiniband->num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%lld", infiniband->events[i], infiniband->values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    return json;
}

static metric
mf_plugin_infiniband_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);
        mf_infiniband_profile(profile_time);
        INFINIBAND_Plugin *infiniband = malloc(sizeof(INFINIBAND_Plugin));
        mf_infiniband_read(infiniband, conf_data->keys);
        strcpy(resMetric->msg, to_JSON(infiniband));
        free(infiniband);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int
init_mf_plugin_infiniband(PluginManager *pm)
{
    PluginManager_register_hook(pm, "mf_plugin_infiniband", mf_plugin_infiniband_hook);

    conf_data = malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_infiniband", conf_data, "on");
    mf_infiniband_init(conf_data->keys, conf_data->size);

    char* value = mfp_get_value("timings", "mf_plugin_infiniband");
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
