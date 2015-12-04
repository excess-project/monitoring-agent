/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ctype.h>
#include <malloc.h>
#include <papi.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_rapl_connector.h"

#define SUCCESS 1
#define FAILURE 0

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

/*
 * forward declarations
 */
static int is_rapl_initialized();
static void initialize_PAPI();

void
initialize_PAPI()
{
    if (is_rapl_initialized()) {
        return;
    }

    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("RAPL:: - PAPI_library_init: %s", error);
        is_initialized = 0;
    }

    is_initialized = 1;
}

int
is_component_enabled()
{
    int numcmp, cid;
    const PAPI_component_info_t *cmpinfo = NULL;

    if (!is_rapl_initialized()) {
        initialize_PAPI();
    }

    if (is_available > -1) {
        return is_available;
    }

    numcmp = PAPI_num_components();
    for (cid = 0; cid < numcmp; cid++) {
        cmpinfo = PAPI_get_component_info(cid);
        if (strstr(cmpinfo->name, "rapl")) {
            if (cmpinfo->disabled) {
                is_available = 0;
                log_info("RAPL >> component is DISABLED for this CPU (%s)", cmpinfo->name);
            } else {
                is_available = 1;
                log_info("RAPL >> component is ENABLED on this the CPU (%s)", cmpinfo->name);
            }
            return is_available;
        }
    }

    is_available = 0;
    return is_available;
}

static int
is_rapl_initialized()
{
    return is_initialized;
}

int
get_available_events(
    RAPL_Plugin *rapl,
    struct timespec profile_interval,
    char **named_events,
    size_t num_events,
    int cpu_model)
{
    long long before_time, after_time;
    double elapsed_time;
    int EventSet = PAPI_NULL;
    int j;

    initialize_PAPI();

    int retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    for (j = 0; j != num_events; ++j) {
        retval = PAPI_add_named_event(EventSet, named_events[j]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_warn("bind_events_to_all_cores() - PAPI_add_named_event (%s): %s",
              named_events[j], error);
        } else {
            log_info("bind_events_to_all_cores() - Added event %s", named_events[j]);
            rapl->events[j] = malloc(PAPI_MAX_STR_LEN + 1);
            strcpy(rapl->events[j], named_events[j]);
        }
    }

    long long *values = calloc(num_events, sizeof(long long));
    if (values == NULL) {
        return -1;
    }
    rapl->num_events = num_events;

    before_time = PAPI_get_real_nsec();
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    nanosleep(&profile_interval, NULL);

    after_time = PAPI_get_real_nsec();
    retval = PAPI_stop(EventSet, values);
    if (retval != PAPI_OK) {
        return -1;
    }

    elapsed_time = ((double)(after_time - before_time)) / 1.0e9;

    int i;
    for (i = 0; i < num_events; ++i) { // average
        rapl->values[i] = ((double) values[i] / 1.0e9) / elapsed_time;
    }

    for (i = 0; i < num_events; ++i) {
        if (strcmp(rapl->events[i], "DRAM_ENERGY:PACKAGE0") == 0 ||
            strcmp(rapl->events[i], "DRAM_ENERGY:PACKAGE1") == 0) {
            if (cpu_model == 15) { /* Haswell */
                rapl->values[i] = ((double) rapl->values[i] / 15.3);
                log_debug("DRAM values adapted, because Haswell processor was detected");
            }
        }
    }
    retval = PAPI_cleanup_eventset(EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    retval = PAPI_destroy_eventset(&EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    return num_events;
}

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

int
get_cpu_model()
{
    unsigned eax, ebx, ecx, edx;
    eax = 1; /* set processor info and feature bits */
    native_cpuid(&eax, &ebx, &ecx, &edx);
    return (eax >> 4) & 0xF;
}

void
mf_rapl_shutdown()
{
    PAPI_shutdown();
}
