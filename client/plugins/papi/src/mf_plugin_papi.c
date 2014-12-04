#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mf_papi_connector.h"
#include "plugin_manager.h"
#include "util.h"

static PAPI_Plugin *papi;

char*
to_JSON(PAPI_Plugin *papi)
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

static metric
papi_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        // will be replaced by a parser call
        char *event_names[2] = {
            "PAPI_L2_DCA",
            "INSTRUCTIONS_RETIRED",
        };
        size_t num_events = 2;

        read_counters(papi, event_names, num_events);
        strcpy(resMetric->msg, to_JSON(papi));

        return resMetric;
    } else {
        return NULL;
    }
}

extern int
init_papi(PluginManager *pm)
{
    PluginManager_register_hook(pm, "papi", papi_hook);

    return 1;
}
