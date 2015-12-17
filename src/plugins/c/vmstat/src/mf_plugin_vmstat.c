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

#include <mf_parser.h> /* mfp_data */
#include <stdlib.h> /* malloc etc */

#include "mf_vmstat_connector.h" /* i.a. VMSTAT_Plugin */
#include "plugin_manager.h" /* mf_plugin_vmstat_hook */

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

mfp_data *conf_data;
int is_available = 0;
VMSTAT_Plugin *monitoring_data = NULL;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static metric mf_plugin_vmstat_hook();

/*******************************************************************************
 * init_mf_plugin_vmstat
 ******************************************************************************/

extern int
init_mf_plugin_vmstat(PluginManager *pm)
{
    /*
     * check if RAPL component is enabled
     */
    is_available = mf_vmstat_is_enabled();
    if (is_available == 0) {
        return 0;
    }

    /*
     * read configuration parameters related to /prov/vmstat (i.e., mf_config.ini)
     */
    PluginManager_register_hook(pm, "mf_plugin_vmstat", mf_plugin_vmstat_hook);
    conf_data =  malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_vmstat", conf_data, "on");

    /*
     * initialize RAPL plug-in including registering metrics
     */
    monitoring_data = malloc(sizeof(VMSTAT_Plugin));
    mf_vmstat_init(monitoring_data, conf_data->keys, conf_data->size);

    return 1;
}


/*******************************************************************************
 * mf_plugin_vmstat_hook
 ******************************************************************************/

static metric
mf_plugin_vmstat_hook()
{
    if (running && (is_available == 1)) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        mf_vmstat_sample(monitoring_data);

        strcpy(resMetric->msg, mf_vmstat_to_json(monitoring_data));

        return resMetric;
    } else {
        return NULL;
    }
}