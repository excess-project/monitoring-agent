#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "ini_parser.h"
#include "mf_papi_connector.h"
#include "plugin_manager.h"
#include "util.h"
#include "excess_main.h"

char*
to_JSON(PAPI_Plugin *papi)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"performance\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < papi->num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%lld", papi->events[i], papi->values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    return json;
}

static metric
mf_plugin_papi_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));
        mfp_data *conf_data = malloc(sizeof(mfp_data));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);
        int sleep_in_ms = 500000; // 0.5s

        mfp_get_data("papi", conf_data);
        mf_papi_init(conf_data->keys, conf_data->size);
        mf_papi_profile(sleep_in_ms);
        PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
        mf_papi_read(papi, conf_data->keys);
        strcpy(resMetric->msg, to_JSON(papi));
        free(papi);
        free(conf_data);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int
init_mf_plugin_papi(PluginManager *pm)
{
    PluginManager_register_hook(pm, "mf_plugin_papi", mf_plugin_papi_hook);

    return 1;
}
