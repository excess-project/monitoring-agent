#include <stdlib.h>

#include "../util.h"
#include "../plugin_manager.h"

#include "likwid_plugin.h"

char* to_JSON(Likwid_Plugin *likwid)
{
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"likwid\"");

    int i;
    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < likwid->numSockets; ++i) {
        sprintf(single_metric, ",\"%s\":\"%s\"", likwid->sockets[i][0], likwid->sockets[i][1]);
        strcat(json, single_metric);
        sprintf(single_metric, ",\"%s\":\"%s\"", likwid->sockets[i][2], likwid->sockets[i][3]);
        strcat(json, single_metric);

        if (likwid->hasDRAM) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->dram[i][0], likwid->dram[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->dram[i][2], likwid->dram[i][3]);
            strcat(json, single_metric);
        }
        if (likwid->hasPP0) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP0[i][0], likwid->PP0[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP0[i][2], likwid->PP0[i][3]);
            strcat(json, single_metric);
        }
        if (likwid->hasPP1) {
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP1[i][0], likwid->PP1[i][1]);
            strcat(json, single_metric);
            sprintf(single_metric, ",\"%s\":\"%s\"", likwid->PP1[i][2], likwid->PP1[i][3]);
            strcat(json, single_metric);
        }
    }

    free(single_metric);

    // where to put free(json) ?

    return json;
}

static metric likwid_hook()
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

        Likwid_Plugin *likwid = malloc(sizeof(Likwid_Plugin));
        get_power_data(likwid, 1);

        strcpy(resMetric->msg, to_JSON(likwid));

        free(likwid);
        //free(json);

        return resMetric;
    } else {
        return NULL;
    }
}

extern int init_likwid(PluginManager *pm)
{
    cpuid_init();


    PluginManager_register_hook(pm, "Likwid", likwid_hook);

    return 1;
}
