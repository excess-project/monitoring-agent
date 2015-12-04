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

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

mfp_data *conf_data;
int is_available = 0;
RAPL_Plugin *monitoring_data = NULL;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static metric mf_plugin_rapl_hook();

/*******************************************************************************
 * init_mf_plugin_rapl
 ******************************************************************************/

extern int
init_mf_plugin_rapl(PluginManager *pm)
{
    /*
     * check if RAPL component is enabled
     */
    is_available = mf_rapl_is_enabled();
    if (is_available == 0) {
        return 0;
    }

    /*
     * read configuration parameters related to RAPL (i.e., mf_config.ini)
     */
    PluginManager_register_hook(pm, "mf_plugin_rapl", mf_plugin_rapl_hook);
    conf_data =  malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_rapl", conf_data, "on");

    /*
     * initialize RAPL plug-in including registering metrics
     */
    monitoring_data = malloc(sizeof(RAPL_Plugin));
    mf_rapl_init(monitoring_data, conf_data->keys, conf_data->size);

    return 1;
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

        mf_rapl_sample(monitoring_data);

        strcpy(resMetric->msg, mf_rapl_to_json(monitoring_data));

        return resMetric;
    } else {
        return NULL;
    }
}