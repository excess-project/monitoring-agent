
#include <ini_parser.h>
#include <stdlib.h>
#include <papi.h>

#include "../util.h"
#include "../plugin_manager.h"
#include "../excess_main.h"

#include "papi_plugin.h"

static mfp_data *conf_papi;

char* to_JSON(PAPI_Plugin *papi)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"papi\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < papi->num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%lld", papi->events[i], papi->values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    return json;
}

static metric papi_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));

        mfp_get_data("papi", conf_papi);
        read_available_named_events(papi, conf_papi->keys, conf_papi->size);

        strcpy(resMetric->msg, to_JSON(papi));

        free(papi);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int init_papi(PluginManager *pm)
{
    PAPI_library_init(PAPI_VER_CURRENT);
    conf_papi = malloc(sizeof(mfp_data));

    PluginManager_register_hook(pm, "papi", papi_hook);

    return 1;
}
