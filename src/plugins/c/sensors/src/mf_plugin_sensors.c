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

#include "mf_sensors_connector.h" /* i.a. SENSORS_Plugin */
#include "plugin_manager.h" /* mf_plugin_sensors_hook */

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/
mfp_data *conf_data;
int is_available = 0;
SENSORS_Plugin *monitoring_data = NULL;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static metric mf_plugin_sensors_hook();

/* Initialize the sensors plugin; register the plugin hook to the plugin manager */
extern int
init_mf_plugin_sensors(PluginManager *pm)
{
    /*
     * check if SENSORS component is enabled
     */
    is_available = mf_sensors_is_enabled();
    if (is_available == 0) {
        return 0;
    }

    /*
     * read configuration parameters related to SENSORS (i.e., mf_config.ini)
     */
    PluginManager_register_hook(pm, "mf_plugin_sensors", mf_plugin_sensors_hook);
    conf_data =  malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_sensors", conf_data, "on");

    /*
     * initialize SENSORS plug-in including registering metrics
     */
    monitoring_data = malloc(sizeof(SENSORS_Plugin));
    mf_sensors_init(monitoring_data, conf_data->keys, conf_data->size);

    mfp_data_free(conf_data);
    return 1;
}


/* Sensors hook function, sample the metrics and convert to a json-formatted string */
static metric
mf_plugin_sensors_hook()
{
    if (running && (is_available == 1)) {
        metric resMetric = malloc(sizeof(metric_t));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        mf_sensors_sample(monitoring_data);

        resMetric->msg = mf_sensors_to_json(monitoring_data);

        return resMetric;
    } else {
        return NULL;
    }
}