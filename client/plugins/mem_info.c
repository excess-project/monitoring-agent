/*
 * mem_info.c example of a plugin
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */
#include <time.h>
//#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"
#include "../plugin_manager.h"
#include "../excess_main.h"

#define BUFFER_SIZE 64*1000;

void getprocmeminfo(unsigned long *restrict mfre, unsigned long *restrict mtot) {

	char line[100];
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (!fp) {
		fprintf(stderr, "/proc/meminfo not found!\n");
		fprintf(logFile, "/proc/meminfo not found!\n");
//		exit(-1);
	}

	while (fgets(line, sizeof(line), fp) != NULL ) {
		char *pos;

		if ((pos = strstr(line, "MemFree: ")))
			sscanf(pos, "MemFree: %100lu kB", mfre);
		if ((pos = strstr(line, "MemTotal: ")))
			sscanf(pos, "MemTotal: %100lu kB", mtot);
	}
	fclose(fp);
}

double get_mem_usage() {

	unsigned long mfre, mtot;

	getprocmeminfo(&mfre, &mtot);

	double frac = (double) mfre / (double) mtot;

	return 100.0 - frac * 100.0;
}
char* toMemData(double usage) {
	char *returnMsg = malloc(250 * sizeof(char));

	double ramUsed = usage;
	double ramAvail = 100.0 - usage;

	sprintf(returnMsg,
			",\"type\":\"mem\",\"mem_used\":\"%.2f\",\"mem_avail\":\"%.2f\"",
			ramUsed, ramAvail);

	return returnMsg;
}

static metric mem_info_hook() {
	if (running) {
		double usage;
		metric resMetric = malloc(sizeof(metric_t));
		resMetric->msg = malloc(100 * sizeof(char));

		int clk_id = CLOCK_REALTIME;
		clock_gettime(clk_id, &resMetric->timestamp);

		usage = get_mem_usage();

		strcpy(resMetric->msg, toMemData(usage));

		return resMetric;
	} else {
		fprintf(stderr, "Shutdown mem_info\n");
				fprintf(logFile, "Shutdown mem_info\n");
		return NULL ;
	}
}

extern int init_mem_info(PluginManager *pm) {

	PluginManager_register_hook(pm, "mem_info", mem_info_hook);
	return 1;
}

