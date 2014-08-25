/*
 * papi.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include "../util.h"
#include "../plugin_manager.h"

#include <stdlib.h>
#include <stdint.h>
#include <papi.h>

#define MAX_PAPI 256

int papiNumbers;

static long_long values[MAX_PAPI];

static int EventSet = PAPI_NULL;

const char *papiEvents[] = { "PAPI_TOT_INS", "PAPI_SP_OPS", "PAPI_TOT_CYC" };

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
	exit(1);
}
int prepare_papi() {
	int retval;
	int i;

//	int EventSet = PAPI_NULL;

	retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT && retval > 0) {
		fprintf(stderr, "getPapiValues: PAPI library version mismatch!\n");
	}

	retval = PAPI_create_eventset(&EventSet);
	if (retval != PAPI_OK) {
		handle_error(retval);
	}

	if (retval < 0) {
		fprintf(stderr, "getPapiValues: Initialization error!\n");
	}
	papiNumbers = sizeof(papiEvents) / sizeof(*papiEvents);

	for (i = 0; i < papiNumbers; i++) {

		retval = PAPI_add_named_event(EventSet,
				(char*) (intptr_t) papiEvents[i]);
		if (retval != PAPI_OK) {
			fprintf(stderr, "getPapiValues: Failure to add PAPI event '%s'.\n",
					(char*) (intptr_t) papiEvents[i]);
			handle_error(retval);
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
	return 1;
}

//long_long* gatherPapi(metric resMetric) {
//
//	return resMetric;
//}

static metric papi_hook() {
	int retval;
	metric resMetric = malloc(sizeof(metric));
	resMetric->msg = malloc(100 * sizeof(char));
	int clk_id = CLOCK_REALTIME;

	retval = PAPI_read(EventSet, values);
	if (retval != PAPI_OK)
		handle_error(retval);

	clock_gettime(clk_id, &resMetric->timestamp);

	strcpy(resMetric->msg, toPapiData(values));

	return resMetric;
}

int init_papi(PluginManager *pm) {
	prepare_papi();
	PluginManager_register_hook(pm, "papi", papi_hook);
	return 1;
}

