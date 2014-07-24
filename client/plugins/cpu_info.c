/*
 * cpu_info.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include "../util.h"
#include "../plugin_manager.h"

static metric cpu_info_hook() {
	metric resMetric = malloc(sizeof(metric));
	resMetric->msg = malloc(100 * sizeof(char));

	/**
	 *
	 * DO SOMETHING TO GATHER METRIC
	 *
	 */

	return resMetric;
}


extern int init_cpu_info(PluginManager *pm) {
	PluginManager_register_hook(pm, cpu_info_hook);
	return 1;
}

