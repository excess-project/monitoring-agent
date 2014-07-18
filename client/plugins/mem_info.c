/*
 * mem_info.c example of a plugin
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */
#include "../util.h"
#include <time.h>

#include "../plugin_manager.h"

//#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 64*1000;

void getprocmeminfo(unsigned long *restrict mfre, unsigned long *restrict mtot) {

	char line[100];
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (!fp) {
		printf("/proc/meminfo not found!\n");
	}

	while (fgets(line, 200, fp) != NULL ) {
		char *pos;

		if ((pos = strstr(line, "MemFree: ")))
			sscanf(pos, "MemFree: %lu kB", mfre);
		if ((pos = strstr(line, "MemTotal: ")))
			sscanf(pos, "MemTotal: %lu kB", mtot);
	}

}

double get_mem_usage() {

	unsigned long mfre, mtot;

	getprocmeminfo(&mfre, &mtot);

	double frac = (double) mfre / (double) mtot;

	return frac * 100.0;
}

static metric_t mem_info_hook() {
	double usage;
	metric_t *resMetric = malloc(sizeof(metric_t));

	int clk_id = CLOCK_REALTIME;
	clock_gettime(clk_id, &resMetric->timestamp);

	usage = get_mem_usage();

	return *resMetric;
}

char* toMemData(double usage) {
	char *returnMsg = malloc(100 * sizeof(char));

	double ramUsed = usage;
	double ramAvail = 100.0 - usage;

	sprintf(returnMsg,
			",\"type\":\"mem\",\"mem_used\":\"%.2f\",\"mem_avail\":\"%.2f\"",
			ramUsed, ramAvail);

	return returnMsg;
}

int init_mem_info(PluginManager *pm) {

	PluginManager_register_hook(pm, mem_info_hook);
	return 1;
}
