/*
 * start_stop.c
 *
 *  Created on: 24.07.2014
 *      Author: hpcneich
 */

#include <stdlib.h>

#include "../util.h"
#include "../plugin_manager.h"

static metric start_stop_hook() {
	metric resMetric = malloc(sizeof(metric));
	resMetric->msg = malloc(100 * sizeof(char));

	/**
	 *
	 * DO SOMETHING TO GATHER METRIC
	 *
	 */
	strcat(resMetric->msg, ",\"type\":\"start\"");
	return resMetric;
}

extern int init_start_stop(PluginManager *pm) {
	PluginManager_register_hook(pm,"start_stop", start_stop_hook);
	return 1;
}

