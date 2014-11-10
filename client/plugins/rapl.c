#include <stdlib.h>
#include <papi.h>

#include "../util.h"
#include "../plugin_manager.h"

#include "rapl_plugin.h"

char* to_JSON(RAPL_Plugin *rapl)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"RAPL\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < rapl->num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%.1f", rapl->events[i], rapl->values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    // where to put free(json) ?

    return json;
}

static metric rapl_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        /*
        char *papi_conf = malloc(300 * sizeof(char));
        papi_conf[0] = '\0';
        strcat(papi_conf, pwd);
        strcat(papi_conf, "/plugins/pluginConf");

        Parser *parser = get_instance();
        read_PAPI_events_from_file(parser, papi_conf);
        */

        RAPL_Plugin *rapl = malloc(sizeof(RAPL_Plugin));
        get_available_events(rapl);

        strcpy(resMetric->msg, to_JSON(rapl));

        free(rapl);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int init_rapl(PluginManager *pm)
{
    PAPI_library_init(PAPI_VER_CURRENT);
    PluginManager_register_hook(pm, "RAPL", rapl_hook);

    return 1;
}
