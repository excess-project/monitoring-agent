/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <pthread.h> /* nanosleep */
#include <stdlib.h> /* malloc */

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_rapl_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

/*
 * declares if the plug-in (i.e., RAPL) is already initialized
 */
static int is_initialized = 0;

/*
 * declares if the RAPL component is enabled to be used for monitoring
 *
 * states: (-1) not initialized, (0) disabled, (1) enabled
 */
static int is_available = -1;

int EventSet = PAPI_NULL;
long long before_time, after_time;
double denominator;
long long *values;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static int is_rapl_initialized();
static int enable_papi_library();
static double mf_rapl_get_denominator();
static double correct_dram_values(char *event, double value);

/*******************************************************************************
 * mf_rapl_is_enabled
 ******************************************************************************/

int
mf_rapl_is_enabled()
{
    int numcmp, cid;
    const PAPI_component_info_t *cmpinfo = NULL;
    enable_papi_library();

    if (is_available > -1) {
        return is_available;
    }

    numcmp = PAPI_num_components();
    for (cid = 0; cid < numcmp; cid++) {
        cmpinfo = PAPI_get_component_info(cid);
        if (strstr(cmpinfo->name, "rapl")) {
            if (cmpinfo->disabled) {
                is_available = FAILURE;
                log_warn("Component is DISABLED for this CPU (%d)", cid);
            } else {
                is_available = SUCCESS;
                log_info("Component is ENABLED (%s)", cmpinfo->name);
            }
            return is_available;
        }
    }

    is_available = FAILURE;
    return is_available;
}

/*******************************************************************************
 * mf_rapl_init
 ******************************************************************************/

int
mf_rapl_init(RAPL_Plugin *data, char **rapl_events, size_t num_events)
{
    /*
     * setup PAPI library
     */
    if (enable_papi_library() != SUCCESS) {
        return FAILURE;
    }

    /*
     * create PAPI EventSet
     */
    int retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        log_error("ERROR: Couldn't create EventSet %s", PAPI_strerror(retval));
        return FAILURE;
    }

    /*
     * add user-defined metrics to the EventSet
     */
    int idx, registered_idx = 0;
    for (idx = 0; idx != num_events; ++idx) {
        retval = PAPI_add_named_event(EventSet, rapl_events[idx]);
        if (retval != PAPI_OK) {
            char *err = PAPI_strerror(retval);
            log_warn("Couldn't add PAPI event (%s): %s", rapl_events[idx], err);
        } else {
            log_info("Added PAPI event %s", rapl_events[idx]);

            /*
             * register added PAPI event at the internal data structure
             */
            data->events[registered_idx] = malloc(PAPI_MAX_STR_LEN + 1);
            strcpy(data->events[registered_idx], rapl_events[idx]);
            registered_idx = registered_idx + 1;
        }
    }
    data->num_events = registered_idx;

    /*
     * set denominator for DRAM values based on the current CPU model
     */
    denominator = mf_rapl_get_denominator();
    values = calloc(registered_idx, sizeof(long long));


    /*
     * start monitoring registered events
     */
    before_time = PAPI_get_real_nsec();
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        log_error("ERROR: Couldn't start monitoring %s", PAPI_strerror(retval));
        return FAILURE;
    }

    return registered_idx;
}

/*******************************************************************************
 * get_available_events
 ******************************************************************************/

int
mf_rapl_sample(RAPL_Plugin *data)
{
    /*
     * initialize array to store monitoring results
     */
    size_t size = data->num_events;
    if (values == NULL) {
        log_error("Couldn't initialize long long values %s", "NULL");
        return FAILURE;
    }

    /*
     * read measurements
     */
    after_time = PAPI_get_real_nsec();
    int retval = PAPI_read(EventSet, values);
    if (retval != PAPI_OK) {
        return FAILURE;
    }

    /*
     * account for time passed between last measurement and now
     */
    int idx;
    double elapsed_time = ((double) (after_time - before_time)) / 1.0e9;
    for (idx = 0; idx < size; ++idx) {
        values[idx] = correct_dram_values(data->events[idx], values[idx]);
        data->values[idx] = ((double) values[idx] / 1.0e9) / elapsed_time;
    }

    /*
     * update time interval
     */
    before_time = after_time;

    return SUCCESS;
}

/*******************************************************************************
 * to_json
 ******************************************************************************/

char*
mf_rapl_to_json(RAPL_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"energy\"");

    int idx;
    size_t size = data->num_events;
    for (idx = 0; idx < size; ++idx) {
        sprintf(metric, ",\"%s\":%.4f", data->events[idx], data->values[idx]);
        strcat(json, metric);
    }
    free(metric);

    return json;
}

/*******************************************************************************
 * correct_dram_values
 ******************************************************************************/

static double
correct_dram_values(char *event, double value)
{
    if (strcmp(event, "DRAM_ENERGY:PACKAGE0") == 0 ||
        strcmp(event, "DRAM_ENERGY:PACKAGE1") == 0) {
        return (double) (value / 15.3);
    }

    return 1.0;
}

/*******************************************************************************
 * enable_papi_library
 ******************************************************************************/

static int
enable_papi_library()
{
    if (is_rapl_initialized()) {
        return is_initialized;
    }

    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("ERROR while initializing PAPI library: %s", error);
        is_initialized = FAILURE;
    } else {
        is_initialized = SUCCESS;
    }

    return is_initialized;
}

/*******************************************************************************
 * is_rapl_initialized
 ******************************************************************************/

static int
is_rapl_initialized()
{
    return is_initialized;
}

/*******************************************************************************
 * mf_rapl_shutdown
 ******************************************************************************/

void
mf_rapl_shutdown()
{
    int retval = PAPI_stop(EventSet, NULL);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("Couldn't stop PAPI EventSet: %s", error);
    }

    retval = PAPI_cleanup_eventset(EventSet);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("Couldn't cleanup PAPI EventSet: %s", error);
    }

    retval = PAPI_destroy_eventset(&EventSet);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("Couldn't destroy PAPI EventSet: %s", error);
    }

    PAPI_shutdown();
}



/*******************************************************************************
 * native_cpuid
 ******************************************************************************/

void
native_cpuid(
    unsigned int *eax,
    unsigned int *ebx,
    unsigned int *ecx,
    unsigned int *edx)
{
    asm volatile("cpuid"
        : "=a" (*eax),
          "=b" (*ebx),
          "=c" (*ecx),
          "=d" (*edx)
        : "0" (*eax), "2" (*ecx)
    );
}

/*******************************************************************************
 * get_cpu_model
 ******************************************************************************/

static int
mf_rapl_get_cpu_model()
{
    unsigned eax, ebx, ecx, edx;
    eax = 1; /* set processor info and feature bits */
    native_cpuid(&eax, &ebx, &ecx, &edx);
    return (eax >> 4) & 0xF;
}

/*******************************************************************************
 * mf_rapl_set_denominator_by
 ******************************************************************************/

static double
mf_rapl_get_denominator()
{
    int cpu_model = mf_rapl_get_cpu_model();
    if (cpu_model == 15) {
        return 15.3;
    } else {
        return 1.0;
    }
}
