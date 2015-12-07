/*
 * Copyright (C) 2014-2015 University of Stuttgart
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
#include "mf_papi_connector.h"

#define SUCCESS 1
#define FAILURE 0

static int DEFAULT_CPU_COMPONENT = 0;
static int domain = PAPI_DOM_ALL;
static int *event_sets = NULL;
static int granularity = PAPI_GRN_SYS;
static int is_initialized = 0;
static int maximum_number_of_cores = 0;
static int *num_events_per_socket = NULL;
static long long **values_per_core = NULL;

void mf_papi_profile();
void mf_papi_read();
static int is_papi_initialized();
static int create_eventset_systemwide();
static int mf_set_affinity();
static void load_papi();
static void get_max_cpus();
static void init_eventsets();
static void create_eventset_for_each_core();
static void bind_events_to_all_cores();
#ifdef DEBUG
static void check_events();
#endif


/*******************************************************************************
 * mf_papi_init
 ******************************************************************************/

int
mf_papi_init(
    PAPI_Plugin *data,
    char **papi_events,
    size_t num_events,
    size_t num_cores)
{
    if (is_papi_initialized()) {
        return SUCCESS;
    }

    if (!load_papi_library()) {
        return FAILURE;
    }

    /*
     * determines the maximum number of available cores for monitoring
     */
    set_maximum_number_of_cores_for_sampling(&num_cores);

    /*
     * creates EventSets for each individual core
     */
    create_eventset_for(num_cores);

    bind_events_to_all_cores(named_events, num_events);
    #ifdef DEBUG
    check_events();
    #endif

    is_initialized = SUCCESS;
    return is_initialized;
}

/*******************************************************************************
 * is_papi_initialized
 ******************************************************************************/

static int
is_papi_initialized()
{
    return is_initialized;
}

/*******************************************************************************
 * load_papi_library
 ******************************************************************************/

static int
load_papi_library()
{
    if (PAPI_is_initialized()) {
        return SUCCESS;
    }

    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while loading the library: %s", error);
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************************************
 * set_maximum_number_of_cores_for_sampling
 ******************************************************************************/

static void
set_maximum_number_of_cores_for_sampling(int *requested_number_of_cores)
{
    /*
     * get number of CPUs
     */
    int *max_cores = PAPI_get_opt(PAPI_MAX_CPUS,/*@-nullpass@*/ NULL);

    /*
     * an error occurred
     */
    if (*max_cores <= 0) {
        char itostr[*max_cores];
        (void) snprintf(itostr, sizeof(itostr) / sizeof(char), "%d", *max_cores);
        log_error("PAPI >> Error while reading PAPI_MAX_CPUS: %s", itostr);
        return;
    }

    /*
     * reduce the number of requested cores for sampling to the maximum
     */
    if (*requested_number_of_cores < *max_cores) {
        if (*requested_number_of_cores > 0) {
            *max_cores = *requested_number_of_cores;
        }
    }

    log_info("PAPI >> Number of cores selected for measuring is %d", *max_cores);
}

/*******************************************************************************
 * create_eventset_for
 ******************************************************************************/

static int
create_eventset_for(int num_cores)
{
    int retval;
    int number_of_core;

    event_sets = malloc(num_cores * sizeof(int));
    if (event_sets == NULL) {
        log_error(
            "PAPI >> Couldn't allocate memory for array of %s",
            "EventSets"
        );
        return FAILURE;
    }

    /*
     * create EventSets for the given number of cores
     *
     * we set the domain to DOM_ALL, and the granularity to system-wide in order
     * to sample events on a system level. this requires root privileges.
     */
    for (number_of_core = 0; number_of_core != num_cores; ++number_of_core) {
        event_sets[number_of_core] = PAPI_NULL; /* set default EventSet to PAPI_NULL */

        int *EventSet = event_sets + number_of_core;
        retval = retval && create(EventSet);
        retval = retval && assign_to_component(EventSet, DEFAULT_CPU_COMPONENT);
        retval = retval && set_domain_for(EventSet, PAPI_DOM_ALL, DEFAULT_CPU_COMPONENT);
        retval = retval && set_granularity_for(EventSet, PAPI_GRN_SYS);
        retval = retval && attach_to_cpu(EventSet, number_of_core);

        log_info("PAPI >> EventSet created for core %d", number_of_core);
    }

    return retval;
}

/*******************************************************************************
 * create_eventset_systemwide
 ******************************************************************************/

static int
init_eventset(int *EventSet, int number_of_core)
{
    int retval;




    return retval;
}

/*******************************************************************************
 * create
 ******************************************************************************/

static int
create(int *EventSet)
{
    int retval = PAPI_create_eventset(*EventSet);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while creating an EventSet: %s", error);
    }

    return retval;
}

/*******************************************************************************
 * assign_to_component
 ******************************************************************************/

static int
assign_to_component(int *EventSet, int cpu_component)
{
    int retval = PAPI_assign_eventset_component(*EventSet, cpu_component);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while assigning EventSet: (%s)", error);
    }

    return retval;
}

/*******************************************************************************
 * set_domain_for
 ******************************************************************************/

static int
set_domain_for(int *EventSet, int domain, int cpu_component)
{
    PAPI_domain_option_t domain_opt;
    domain_opt.def_cidx = cpu_component;
    domain_opt.eventset = *EventSet;
    domain_opt.domain = domain;

    int retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*) &domain_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while setting PAPI_DOMAIN: %s", error);
    }

    return retval;
}

/*******************************************************************************
 * set_granularity_for
 ******************************************************************************/

static int
set_granularity_for(int *EventSet, int granularity)
{
    PAPI_granularity_option_t gran_opt;
    gran_opt.eventset = *EventSet;
    gran_opt.granularity = granularity;

    int retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*) &gran_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while setting PAPI_GRANUL: %s", error);
    }

    return retval;
}

/*******************************************************************************
 * attach_to_cpu
 ******************************************************************************/

static int
attach_to_cpu(EventSet, number_of_core)
{
    PAPI_cpu_option_t cpu_opt;
    cpu_opt.eventset = *EventSet;
    cpu_opt.cpu_num = number_of_core;

    int retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*) &cpu_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while attaching to CPU %d: %s",
            number_of_core,
            error
        );
    }

    return retval;
}

static int
mf_set_affinity(int thread_id)
{
    int retval;
    cpu_set_t processor_mask;

    CPU_ZERO(&processor_mask);
    CPU_SET(thread_id,&processor_mask);
    retval = sched_setaffinity(0, sizeof(cpu_set_t), &processor_mask);

    return retval;
}

static void
bind_events_to_all_cores(char **named_events, size_t num_events)
{
    int i, j;
    int retval;

    num_events_per_socket = malloc(num_cores * sizeof(int));
    for (i = 0; i != num_cores; ++i) {
        num_events_per_socket[i] = 0;
        for (j = 0; j != num_events; ++j) {
            retval = PAPI_add_named_event(event_sets[i], named_events[j]);
            if (retval != PAPI_OK) {
                char *error = PAPI_strerror(retval);
                log_warn("bind_events_to_all_cores() - PAPI_add_named_event (%s): %s",
                    named_events[j], error);
            } else {
                num_events_per_socket[i]++;
                log_info("bind_events_to_all_cores() - Added event %s to CPU%d",
                    named_events[j], i);
            }
        }
    }
}

#ifdef DEBUG
static void
check_events()
{
    if (num_events_per_socket == NULL) {
        log_warn("check_events() - num_events_per_socket not %s", "initialized");
        return;
    }

    for (int i = 0; i != num_cores; ++i) {
        if (num_events_per_socket[i] == 0) {
            log_warn("check_events() - No events added for %s", "monitoring");
        }
    }
}
#endif

void
mf_papi_profile(struct timespec profile_interval)
{
    int i;
    int retval;
    values_per_core = malloc(num_cores * sizeof(long long *));

    for (i = 0; i != num_cores; ++i) {
        debug("Start PAPI Monitoring for CPU%d", i);
        retval = PAPI_start(event_sets[i]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("mf_papi_profile() - PAPI_start: %s", error);
        }
    }

    nanosleep(&profile_interval, NULL);

    for (i = 0; i != num_cores; ++i) {
        debug("Stop PAPI Monitoring for CPU%d", i);
        values_per_core[i] = malloc(num_events_per_socket[i] * sizeof(long long));
        retval = PAPI_stop(event_sets[i], values_per_core[i]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("mf_papi_profile() - PAPI_stop: %s", error);
        }
    }
}

void
mf_papi_read(PAPI_Plugin *papi, char **named_events)
{
    int i, j;

    if (papi == NULL) {
        papi = malloc(sizeof(PAPI_Plugin));
    }
    memset(papi, 0, sizeof(PAPI_Plugin));
    papi->num_events = 0;

    // FIXME: If not all events are available on each core, then the order of
    //        events for at least one core differs. As a result, the associated
    //        event names will be wrong. We need a third variable storing this
    //        information (successfully added or not).
    for (i = 0; i != num_cores; ++i) {
        for (j = 0; j != num_events_per_socket[i]; ++j) {
            papi->events[papi->num_events] = malloc(PAPI_MAX_STR_LEN + 1);
            char metric_name[PAPI_MAX_STR_LEN];
            sprintf(metric_name, "CPU%d::%s", i, named_events[j]);
            strcpy(papi->events[papi->num_events], metric_name);
            papi->values[papi->num_events] = values_per_core[i][j];

            debug("mf_papi_read() - %s=%lld",
                papi->events[papi->num_events],
                papi->values[papi->num_events]
            );

            papi->num_events++;
        }
    }
}

/*******************************************************************************
 * mf_papi_shutdown
 ******************************************************************************/

void
mf_papi_shutdown()
{
    PAPI_shutdown();
}
