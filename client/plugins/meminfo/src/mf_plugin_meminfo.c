/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "plugin_manager.h"
#include "excess_main.h"

#define BUFFER_SIZE 64*1000;

mfp_data *conf_data;

void
getprocmeminfo(unsigned long *restrict mfre, unsigned long *restrict mtot) {
	char line[100];
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (!fp) {
		fprintf(stderr, "/proc/meminfo not found!\n");
		fprintf(logFile, "/proc/meminfo not found!\n");
	}

	while (fgets(line, sizeof(line), fp) != NULL ) {
		char *pos;
		if ((pos = strstr(line, "MemFree: "))) {
			sscanf(pos, "MemFree: %100lu kB", mfre);
		}
		if ((pos = strstr(line, "MemTotal: "))) {
			sscanf(pos, "MemTotal: %100lu kB", mtot);
		}
	}

	fclose(fp);
}

double
get_mem_usage() {
	unsigned long mfre, mtot;
	getprocmeminfo(&mfre, &mtot);
	double frac = (double) mfre / (double) mtot;

	return 100.0 - frac * 100.0;
}

char*
toMemData(double usage) {
	char *returnMsg = malloc(250 * sizeof(char));
	double ram_used = usage;
	double ram_avail = 100.0 - usage;

	char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"memory\"");

    int i;
	for (i = 0; i != conf_data->size; ++i) {
		if (strcmp(conf_data->keys[i], "mem_used") == 0) {
			sprintf(returnMsg, ",\"mem_used\":%.2f", ram_used);
			strcat(json, returnMsg);
		}
		if (strcmp(conf_data->keys[i], "mem_avail") == 0) {
			sprintf(returnMsg, ",\"mem_avail\":%.2f", ram_avail);
			strcat(json, returnMsg);
		}
	}

	return json;
}

static metric
mf_plugin_meminfo_hook() {
	if (running) {
		metric resMetric = malloc(sizeof(metric_t));
		resMetric->msg = malloc(100 * sizeof(char));

		int clk_id = CLOCK_REALTIME;
		clock_gettime(clk_id, &resMetric->timestamp);

		double usage = get_mem_usage();
		strcpy(resMetric->msg, toMemData(usage));

		return resMetric;
	} else {
		return NULL ;
	}
}

extern int
init_mf_plugin_meminfo(PluginManager *pm) {
	PluginManager_register_hook(pm, "mf_plugin_meminfo", mf_plugin_meminfo_hook);

	conf_data = malloc(sizeof(mfp_data));
    mfp_get_data_filtered_by_value("mf_plugin_meminfo", conf_data, "on");

	return 1;
}

