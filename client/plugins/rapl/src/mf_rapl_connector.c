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

#include "mf_debug.h"
#include "mf_rapl_connector.h"


#define SUCCESS 1
#define FAILURE 0

static int is_initialized;
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
    }
}

static int
is_rapl_initialized()
{
    return is_initialized;
}

//fangli changed get_available_events
int
get_available_events(RAPL_Plugin *rapl, struct timespec profile_interval, char **named_events, size_t num_events, int cpu_model)
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

    //rapl->values = calloc(num_events, sizeof(long long));
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

    /* fangli
    if model=14, it is node01, node02 (Ivy bridge processor)
    if model=15, it is node03 (Hasswel processor)
    for Hasswel processor, DRAM_ENERGY need to be divided by 15.3 (for unit difference) */
    for (i = 0; i < num_events; ++i) {
        if (strcmp (rapl->events[i], "DRAM_ENERGY:PACKAGE0")== 0 || strcmp (rapl->events[i], "DRAM_ENERGY:PACKAGE1")== 0) {
            if(cpu_model == 15) {
                rapl->values[i] = ((double) rapl->values[i] / 15.3);
            }
            else {
                printf("cpu_model is not Hasswel\n");
            }
        }
    } //fangli
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
mf_rapl_shutdown()
{
    PAPI_shutdown();
}
