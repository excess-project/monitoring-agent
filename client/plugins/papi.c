/*
 * papi.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */



#include <stdlib.h>
#include <stdint.h>
#include <papi.h>
#include <stdio.h>

#include "../util.h"
#include "../plugin_manager.h"
#include "../excess_main.h"

#define MAX_PAPI 256

int papiNumbers;

static long_long values[MAX_PAPI];

static int EventSet = PAPI_NULL;

//const char *papiEvents[] = { "PAPI_TOT_INS", "PAPI_SP_OPS", "PAPI_TOT_CYC" };
char *papiEvents[256];

char* toPapiData(long_long *val) {
	char *returnMsg = malloc(200 * sizeof(char));
	strcpy(returnMsg, ",\"type\":\"papi\"");

	char *msgPart = malloc(200 * sizeof(char));

	for (int i = 0; i < papiNumbers; i++) {
		sprintf(msgPart, ",\"%s\":\"%lld\"", papiEvents[i], val[i]);
		strcat(returnMsg, msgPart);
	}
	free(msgPart);
	return returnMsg;
}
void handle_error(int err) {
	fprintf(stderr, "papi-pcm.c: Failure with PAPI error '%s'.\n",
			PAPI_strerror(err));
	fprintf(logFile, "papi-pcm.c: Failure with PAPI error '%s'.\n",
			PAPI_strerror(err));
	exit(1);
}

int readPapiConf() {

	char *papiConfLocation = malloc(300 * sizeof(char));
	papiConfLocation[0] = '\0';
	strcat(papiConfLocation, pwd);
	strcat(papiConfLocation, "/plugins/pluginConf");
	char line[200];
	FILE *papiConf = fopen(papiConfLocation, "r");
	if (!papiConf) {
		fprintf(stderr, "File not found!\n%s\n using default events\n",
				confFile);
		fprintf(logFile, "File not found!\n%s\n using default events\n",
				confFile);
	}
	while (fgets(line, 200, papiConf) != NULL ) {
		char *pos;
		if ((pos = strstr(line, "#"))) {
			continue;
		}
		if ((pos = strstr(line, "Events: "))) {
			int where = 0;
			char *p = NULL;
			p = strtok(pos + strlen("Events:") + 1, ",");
			while (p) {
				papiEvents[where] = malloc(256);
				strcpy(papiEvents[where], p);
				p = strtok(NULL, ",");
				if (papiEvents[where][strlen(papiEvents[where]) - 1] == '\n')
					papiEvents[where][strlen(papiEvents[where]) - 1] = '\0';
				where++;
			}
			papiNumbers = where;
			continue;
		}

	}

	fclose(papiConf);
	free(papiConfLocation);
	return 1;
}
int prepare_papi() {
	int retval;
	int i;
	readPapiConf();
//	int EventSet = PAPI_NULL;

	retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT && retval > 0) {
		fprintf(stderr, "getPapiValues: PAPI library version mismatch!\n");
		fprintf(logFile, "getPapiValues: PAPI library version mismatch!\n");
	}

	retval = PAPI_create_eventset(&EventSet);
	if (retval != PAPI_OK) {
		handle_error(retval);
	}

	if (retval < 0) {
		fprintf(stderr, "getPapiValues: Initialization error!\n");
		fprintf(logFile, "getPapiValues: Initialization error!\n");
	}

	for (i = 0; i < papiNumbers; i++) {

		retval = PAPI_add_named_event(EventSet,
				(char*) (intptr_t) papiEvents[i]);
		if (retval != PAPI_OK) {
			fprintf(stderr,
					"getPapiValues: Failure to add PAPI event '%s'.\nskipping...\n",
					(char*) (intptr_t) papiEvents[i]);
			fprintf(logFile,
					"getPapiValues: Failure to add PAPI event '%s'.\nskipping...\n",
					(char*) (intptr_t) papiEvents[i]);
//			handle_error(retval);
		}
	}

	retval = PAPI_start(EventSet);
	if (retval != PAPI_OK) {
		handle_error(retval);

	}

//	gatherPapiData(&EventSet, &values);
	return 1;
}

int afterPapi() {
	PAPI_stop(EventSet, values);
	PAPI_shutdown();
	return 1;
}

//long_long* gatherPapi(metric resMetric) {
//
//	return resMetric;
//}

static metric papi_hook() {
	if (running) {
		int retval;
		metric resMetric = malloc(sizeof(metric_t));
		resMetric->msg = malloc(100 * sizeof(char));
		int clk_id = CLOCK_REALTIME;

		retval = PAPI_read(EventSet, values);
		if (retval != PAPI_OK)
			handle_error(retval);

		clock_gettime(clk_id, &resMetric->timestamp);

		strcpy(resMetric->msg, toPapiData(values));

		return resMetric;
	} else {
		PAPI_shutdown();
		fprintf(stderr, "Shutdown papi!\n");
		fprintf(logFile, "Shutdown papi!\n");
		return NULL ;
	}
}

int init_papi(PluginManager *pm) {
	prepare_papi();
	PluginManager_register_hook(pm, "papi", papi_hook);
	return 1;
}

