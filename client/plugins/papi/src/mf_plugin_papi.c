#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mf_papi_connector.h"
#include "plugin_manager.h"
#include "util.h"

char*
to_JSON(PAPI_Plugin *papi)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"performance\"");

    printf("toJSON() - %d", papi->num_events);

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

        int clk_id = CLOCK_REALTIME;
        int retval;

        clock_gettime(clk_id, &resMetric->timestamp);

        // will be replaced with a parser call
        int sleep_in_ms = 500000; // 0.5s
        char *named_events[2] = {
            "PAPI_L2_DCA",
            "PAPI_TOT_INS",
        };
        size_t num_events = 2;

        retval = mf_papi_init(named_events, num_events);
        if (retval != PAPI_OK) {
            return NULL;
        }
        mf_papi_profile(sleep_in_ms);
        PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
        mf_papi_read(papi, named_events);
        strcpy(resMetric->msg, to_JSON(papi));
        free(papi);

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
