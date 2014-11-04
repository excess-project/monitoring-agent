#include <stdlib.h>
#include <papi.h>

#include "../util.h"
#include "../plugin_manager.h"
#include "../excess_main.h"

#include "papi_plugin.h"
#include "parser.h"

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

        char *papi_conf = malloc(300 * sizeof(char));
        papi_conf[0] = '\0';
        strcat(papi_conf, pwd);
        strcat(papi_conf, "/plugins/pluginConf");

        Parser *parser = get_instance();
        read_PAPI_events_from_file(parser, papi_conf);

        PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
        read_available_named_events(papi, parser->events, parser->metrics_count);

        strcpy(resMetric->msg, to_JSON(papi));

        free(papi);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int init_papi(PluginManager *pm)
{
    PluginManager_register_hook(pm, "papi", papi_hook);

    return 1;
}
