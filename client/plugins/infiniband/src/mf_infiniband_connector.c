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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <malloc.h>
#include <papi.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mf_debug.h"
#include "mf_infiniband_connector.h"

#define SUCCESS 1
#define FAILURE 0

static int event_set = NULL;
static int num_events_counted;
static long long *values;
static int is_initialized;

void mf_infiniband_profile();
void mf_infiniband_read();
static int is_infiniband_initialized();
static void load_papi();
static void bind_events();

void
mf_infiniband_init(char **named_events, size_t num_events)
{
    if (is_infiniband_initialized()) {
        return;
    }

    load_papi();
    PAPI_create_eventset(&event_set);
    bind_events(named_events, num_events);

    is_initialized = 1;
}

static int
is_infiniband_initialized()
{
    return is_initialized;
}

static void
load_papi()
{
    if (PAPI_is_initialized()) {
        return;
    }

    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("load_papi() - PAPI_library_init: %s", error);
    }
}

void
mf_infiniband_shutdown()
{
    PAPI_shutdown();
}

static void
bind_events(char **named_events, size_t num_events)
{
    int i;
    int retval;
    num_events_counted = 0;

    for (i = 0; i != num_events; ++i) {
        retval = PAPI_add_named_event(event_set, named_events[i]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_warn("bind_events() - PAPI_add_named_event (%s): %s",
                named_events[i], error);
        } else {
            log_info("bind_events() - Added event %s", named_events[i]);
            num_events_counted++;
        }
    }

    values = calloc(num_events, sizeof(long long));
}

void
mf_infiniband_profile(struct timespec profile_interval)
{
    debug("Start INFINIBAND Monitoring %d", 0);
    int retval = PAPI_start(event_set);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("mf_infiniband_profile() - PAPI_start: %s", error);
    }

    nanosleep(&profile_interval, NULL);

    debug("Stop INFINIBAND Monitoring %d", 0);
    retval = PAPI_stop(event_set, values);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("mf_infiniband_profile() - PAPI_stop: %s", error);
    }
}

void
mf_infiniband_read(INFINIBAND_Plugin *infiniband, char **named_events)
{
    int j;

    if (infiniband == NULL) {
        infiniband = malloc(sizeof(INFINIBAND_Plugin));
    }
    memset(infiniband, 0, sizeof(INFINIBAND_Plugin));
    infiniband->num_events = 0;

    for (j = 0; j != num_events_counted; ++j) {
        infiniband->events[infiniband->num_events] = malloc(PAPI_MAX_STR_LEN + 1);
        strcpy(infiniband->events[infiniband->num_events], named_events[j]);
        infiniband->values[infiniband->num_events] = values[j];

        debug("mf_infiniband_read() - %s=%lld",
            infiniband->events[infiniband->num_events],
            infiniband->values[infiniband->num_events]
        );

        infiniband->num_events++;
    }
}