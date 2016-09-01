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

#include "mf_movidius_arduino_connector.h" /* i.a. MOVI_Plugin */
#include "plugin_manager.h" /* mf_plugin_movi_hook */

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

mfp_data *conf_data;
MOVI_Plugin *monitoring_data = NULL;
int is_available = 0;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static metric mf_plugin_movi_hook();

/* Initialize the movidius plugin; register the plugin hook to the plugin manager */
extern int
init_mf_plugin_movidius_arduino(PluginManager *pm)
{
    /*
     * read configuration parameters related to RAPL (i.e., mf_config.ini)
     */
    PluginManager_register_hook(pm, "mf_plugin_movidius_arduino", mf_plugin_movi_hook);
    conf_data =  malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_movidius_arduino", conf_data, "on");

    int num_cores = 1;
    /*
     * initialize MOVI plug-in including registering metrics
     */
    monitoring_data = malloc(num_cores * sizeof(MOVI_Plugin));
    is_available = mf_movi_init(monitoring_data, conf_data->keys, conf_data->size);

    return is_available;
}

/* Movidius hook function, sample the metrics and convert to a json-formatted string */
static metric
mf_plugin_movi_hook()
{
    if (running && (is_available == 1)) {
        metric resMetric = malloc(sizeof(metric_t));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        mf_movi_sample(monitoring_data);

        resMetric->msg = mf_movi_to_json(monitoring_data);

        return resMetric;
    } else {
        return NULL;
    }
}
