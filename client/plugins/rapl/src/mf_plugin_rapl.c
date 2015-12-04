/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <mf_parser.h> /* mfp_data */
#include <stdlib.h> /* malloc etc */

#include "mf_rapl_connector.h" /* i.a. RAPL_Plugin */
#include "plugin_manager.h" /* mf_plugin_rapl_hook */

struct timespec profile_time = { 0, 0 };
mfp_data *conf_data;
int cpu_model;
int is_available = 0;

/*******************************************************************************
 * to_JSON
 ******************************************************************************/

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

/*******************************************************************************
 * mf_plugin_rapl_hook
 ******************************************************************************/

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

/*******************************************************************************
 * init_mf_plugin_rapl
 ******************************************************************************/

extern int
init_mf_plugin_rapl(PluginManager *pm)
{
    is_available = is_component_enabled();
    cpu_model = get_cpu_model();

    /*
     * read configuration parameters related to RAPL (i.e., mf_config.ini)
     */
    PluginManager_register_hook(pm, "mf_plugin_rapl", mf_plugin_rapl_hook);
    conf_data =  malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_rapl", conf_data, "on");

    /*
     * set interval for measuring data
     */
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
