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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "excess_main.h"
#include "mf_debug.h"
#include "mf_iostat_connector.h"
#include "plugin_manager.h"
#include "util.h"

char*
to_JSON(Iostat_Plugin *iostat)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"performance\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < iostat->num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%f", iostat->events[i], iostat->values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    return json;
}

static metric
mf_plugin_iostat_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        Iostat_Plugin *iostat = malloc(sizeof(Iostat_Plugin));
        mf_iostat_read(iostat);
        strcpy(resMetric->msg, to_JSON(iostat));
        free(iostat);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int
init_mf_plugin_iostat(PluginManager *pm)
{
    PluginManager_register_hook(pm, "mf_plugin_iostat", mf_plugin_iostat_hook);

    mfp_data *conf_data = malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_iostat", conf_data, "on");

    mf_iostat_init(conf_data->keys, conf_data->size);

    return 1;
}
