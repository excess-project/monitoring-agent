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

#include <cpuid.h>
#include <mf_parser.h>
#include <stdlib.h>

#include "mf_likwid_connector.h"
#include "plugin_manager.h"
#include "util.h"

struct timespec profile_time = { 0, 0 };
mfp_data *conf_data;

char*
to_JSON(Likwid_Plugin *likwid)
{
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"energy\"");

    int i;
    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < likwid->numSockets; ++i) {
        if (likwid->hasPKG) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->sockets[i][0], likwid->sockets[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->sockets[i][2], likwid->sockets[i][3]);
            strcat(json, single_metric);
        }
        if (likwid->hasDRAM) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->dram[i][0], likwid->dram[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->dram[i][2], likwid->dram[i][3]);
            strcat(json, single_metric);
        }
        if (likwid->hasPP0) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP0[i][0], likwid->PP0[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP0[i][2], likwid->PP0[i][3]);
            strcat(json, single_metric);
        }
        if (likwid->hasPP1) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP1[i][0], likwid->PP1[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP1[i][2], likwid->PP1[i][3]);
            strcat(json, single_metric);
        }
    }

    free(single_metric);
    return json;
}

static metric
mf_plugin_likwid_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        Likwid_Plugin *likwid = malloc(sizeof(Likwid_Plugin));
        get_power_data(likwid, conf_data->keys, conf_data->size, profile_time);
        strcpy(resMetric->msg, to_JSON(likwid));

        free(likwid);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int
init_mf_plugin_likwid(PluginManager *pm)
{
    cpuid_init();
    PluginManager_register_hook(pm, "mf_plugin_likwid", mf_plugin_likwid_hook);

    conf_data = malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_likwid", conf_data, "on");

    char* value = mfp_get_value("timings", "mf_plugin_likwid");
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
