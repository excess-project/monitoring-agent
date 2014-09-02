/*
 * cpu_info.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include <stdlib.h>

#include "../util.h"
#include "../plugin_manager.h"

static metric cpu_info_hook() {
	if (running) {
		metric resMetric = malloc(sizeof(metric_t));
		resMetric->msg = malloc(100 * sizeof(char));

		int clk_id = CLOCK_REALTIME;
		clock_gettime(clk_id, &resMetric->timestamp);
		/**
		 *
		 * DO SOMETHING TO GATHER METRIC
		 *
		 */
		strcat(resMetric->msg, ",\"type\":\"cpu\"");
		return resMetric;
	} else {
		return NULL ;
	}
}

extern int init_cpu_info(PluginManager *pm) {
	PluginManager_register_hook(pm, "cpu_info", cpu_info_hook);
	return 1;
}

