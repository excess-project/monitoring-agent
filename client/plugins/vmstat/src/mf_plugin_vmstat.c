#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "plugin_manager.h"
#include "excess_main.h"
#include "mf_vmstat_connector.h"

struct timespec profile_time = { 0, 0 };
mfp_data *conf_data;

static metric
mf_plugin_vmstat_hook() {
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        vmstat_plugin* vmstat = malloc(sizeof(vmstat_plugin));
        mf_vmstat_read(vmstat, conf_data->keys, conf_data->size);
        strcpy(resMetric->msg, mf_vmstat_tojson(vmstat));

        return resMetric;
    } else {
        return NULL ;
    }
}

extern int
init_mf_plugin_vmstat(PluginManager *pm) {
    PluginManager_register_hook(pm, "mf_plugin_vmstat", mf_plugin_vmstat_hook);

    conf_data = malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_vmstat", conf_data, "on");

    return 1;
}