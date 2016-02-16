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

#include "mf_papi_connector.h" /* i.a. PAPI_Plugin */
#include "plugin_manager.h" /* mf_plugin_papi_hook */

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

mfp_data *conf_data;
PAPI_Plugin **monitoring_data = NULL;
int is_available = 0;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static metric mf_plugin_papi_hook();

/*******************************************************************************
 * init_mf_plugin_papi
 ******************************************************************************/

extern int
init_mf_plugin_papi(PluginManager *pm)
{
    /*
     * read configuration parameters related to RAPL (i.e., mf_config.ini)
     */
    PluginManager_register_hook(pm, "mf_plugin_papi", mf_plugin_papi_hook);
    conf_data =  malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_papi", conf_data, "on");

    int num_cores = 1;
    char str_num_cores[10]={'\0'};
    mfp_get_value("mf_plugin_papi", "MAX_CPU_CORES", str_num_cores);
    //char* str_num_cores = mfp_get_value("mf_plugin_papi", "MAX_CPU_CORES");
    if (strcmp(str_num_cores, "MAX") != 0) {
        num_cores = atoi(str_num_cores);
    }

    /*
     * initialize RAPL plug-in including registering metrics
     */
    monitoring_data = malloc(num_cores * sizeof(*monitoring_data));
    is_available = mf_papi_init(
        monitoring_data,
        conf_data->keys,
        conf_data->size,
        num_cores
    );

    return is_available;
}

/*******************************************************************************
 * mf_plugin_rapl_hook
 ******************************************************************************/

static metric
mf_plugin_papi_hook()
{
    if (running && (is_available == 1)) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        mf_papi_sample(monitoring_data);

        strcpy(resMetric->msg, mf_papi_to_json(monitoring_data));

        return resMetric;
    } else {
        return NULL;
    }
}