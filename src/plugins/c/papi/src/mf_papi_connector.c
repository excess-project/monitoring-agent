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

#include <stdlib.h> /* malloc */

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_papi_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * VARIABLE DECLARATIONS
 ******************************************************************************/

static int DEFAULT_CPU_COMPONENT = 0;
static int *event_sets = NULL;
static int is_initialized = 0;
static int maximum_number_of_cores = 1;
static long long *before_time, *after_time;

/*******************************************************************************
 * FORWARD DECLARATIONS
 ******************************************************************************/

static int is_papi_initialized();
static int load_papi_library();
static void set_maximum_number_of_cores_for_sampling(
    size_t *requested_number_of_cores
);
static int create_eventset_for();
static void bind_events_to_cores(
    PAPI_Plugin **data,
    char **papi_events,
    size_t num_events,
    size_t num_cores
);
static int create_new_eventset();
static int assign_to_component();
static int set_domain_for();
static int set_granularity_for();
static int attach_to_cpu();

/*******************************************************************************
 * mf_papi_init
 ******************************************************************************/

int
mf_papi_init(
    PAPI_Plugin **data,
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
    maximum_number_of_cores = num_cores;

    /*
     * creates EventSets for each individual core
     */
    create_eventset_for(num_cores);

    /*
     * bind RAPL events to up to @p num_cores
     */
    bind_events_to_cores(data, papi_events, num_events, num_cores);

    /*
     * initialize time measurements
     */
    before_time = malloc(sizeof(long long) * num_cores);
    after_time = malloc(sizeof(long long) * num_cores);

    /*
     * start the PAPI counters
     */
    int core;
    int retval;
    for (core = 0; core != num_cores; ++core) {
        before_time[core] = PAPI_get_real_nsec();
        retval = PAPI_start(event_sets[core]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("PAPI >> Error while trying to start events: %s", error);
            return FAILURE;
        }
    }

    return SUCCESS;
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
set_maximum_number_of_cores_for_sampling(size_t *requested_number_of_cores)
{
    /*
     * get number of CPUs
     */
    int max_cores = PAPI_get_opt(PAPI_MAX_CPUS,/*@-nullpass@*/ NULL);
    printf("MAX CORES %zu\n", *requested_number_of_cores);
    /*
     * an error occurred
     */
    if (max_cores <= 0) {
        char itostr[max_cores];
        (void) snprintf(itostr, sizeof(itostr) / sizeof(char), "%d", max_cores);
        log_error("PAPI >> Error while reading PAPI_MAX_CPUS: %s", itostr);
        return;
    }

    /*
     * reduce the number of requested cores for sampling to the maximum
     */
    if (*requested_number_of_cores > max_cores) {
        *requested_number_of_cores = max_cores;
    }

    log_info("PAPI >> Number of cores selected for measuring is %d", max_cores);
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

        retval = create_new_eventset(&event_sets[number_of_core]);
        if (retval != PAPI_OK) {
            return FAILURE;
        }

        retval = assign_to_component(event_sets[number_of_core], DEFAULT_CPU_COMPONENT);
        if (retval != PAPI_OK) {
            return FAILURE;
        }

        retval = set_domain_for(event_sets[number_of_core], PAPI_DOM_ALL, DEFAULT_CPU_COMPONENT);
        if (retval != PAPI_OK) {
            return FAILURE;
        }

        retval = set_granularity_for(event_sets[number_of_core], PAPI_GRN_SYS);
        if (retval != PAPI_OK) {
            return FAILURE;
        }

        retval = attach_to_cpu(event_sets[number_of_core], number_of_core);
        if (retval != PAPI_OK) {
            return FAILURE;
        }

        log_info("PAPI >> EventSet created for core %d", number_of_core);
    }

    return retval;
}

/*******************************************************************************
 * create_new_eventset
 ******************************************************************************/

static int
create_new_eventset(int *EventSet)
{
    int retval = PAPI_create_eventset(EventSet);
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
assign_to_component(int EventSet, int cpu_component)
{
    int retval = PAPI_assign_eventset_component(EventSet, cpu_component);
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
set_domain_for(int EventSet, int domain, int cpu_component)
{
    PAPI_domain_option_t domain_opt;
    domain_opt.def_cidx = cpu_component;
    domain_opt.eventset = EventSet;
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
set_granularity_for(int EventSet, int granularity)
{
    PAPI_granularity_option_t gran_opt;
    gran_opt.eventset = EventSet;
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
attach_to_cpu(int EventSet, int number_of_core)
{
    PAPI_cpu_option_t cpu_opt;
    cpu_opt.eventset = EventSet;
    cpu_opt.cpu_num = number_of_core;

    int retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*) &cpu_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("PAPI >> Error while attaching to CPU: %s", error);
    }

    return retval;
}

/*******************************************************************************
 * bind_events_to_cores
 ******************************************************************************/

static void
bind_events_to_cores(
    PAPI_Plugin **data,
    char **papi_events,
    size_t num_events,
    size_t num_cores)
{
    int retval;
    int core;
    int event_idx;
    int registered_events;

    for (core = 0; core != num_cores; ++core) {
        /*
         * initialize new data structure per core
         */
        data[core] = malloc(sizeof(PAPI_Plugin));
        registered_events = 0;

        for (event_idx = 0; event_idx != num_events; ++event_idx) {
            retval = PAPI_add_named_event(
                event_sets[core],
                papi_events[event_idx]
            );

            /*
             * failed to add a PAPI event
             */
            if (retval != PAPI_OK) {
                char *error_message = PAPI_strerror(retval);
                log_error("PAPI >> Error while adding a PAPI event (%s): %s",
                    papi_events[event_idx],
                    error_message
                );
                continue;
            }

            log_info("Added PAPI event %s", papi_events[event_idx]);

            /*
             * prefix metric name with core number
             */
            data[core]->events[registered_events] = malloc(PAPI_MAX_STR_LEN + 1);
            char metric_name[PAPI_MAX_STR_LEN];
            sprintf(metric_name, "CPU%d::%s", core, papi_events[event_idx]);

            /*
             * register PAPI event at the internal data structure
             */
            strcpy(data[core]->events[registered_events], metric_name);
            registered_events = registered_events + 1;
        }

        data[core]->num_events = registered_events;
    }
}

/*******************************************************************************
 * mf_papi_sample
 ******************************************************************************/

int
mf_papi_sample(PAPI_Plugin **data)
{
    int idx;
    int core;
    int retval;
    double elapsed_time;

    for (core = 0; core != maximum_number_of_cores; ++core) {
        /*
         * compute time interval used for sampling
         */
        after_time[core] = PAPI_get_real_nsec();

        /*
         * read, set, and reset counters
         */
        retval = PAPI_read(event_sets[core], data[core]->values);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("PAPI >> Error while reading PAPI counter: %s", error);
        }

        elapsed_time = ((double) (after_time[core] - before_time[core])) / 1.0e9;
        for (idx = 0; idx != data[core]->num_events; ++idx) {
            data[core]->values[idx] = data[core]->values[idx] / elapsed_time;
        }

        /*
         * update time interval
         */
        before_time[core] = after_time[core];

        /*
         * reset counters to zero for next sample interval
         */
        PAPI_reset(event_sets[core]);
    }

    return retval;
}

/*******************************************************************************
 * mf_papi_to_json
 ******************************************************************************/

char*
mf_papi_to_json(PAPI_Plugin **data)
{
    int core, event_idx;
    size_t num_events;

    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(2048 * sizeof(char));
    strcpy(json, "\"type\":\"performance\"");

    for (core = 0; core != maximum_number_of_cores; ++core) {
        num_events = data[core]->num_events;
        for (event_idx = 0; event_idx != num_events; ++event_idx) {
            sprintf(metric, ",\"%s\":%lld",
                data[core]->events[event_idx],
                data[core]->values[event_idx]
            );
            strcat(json, metric);
        }
    }
    free(metric);

    return json;
}

/*******************************************************************************
 * mf_papi_shutdown
 ******************************************************************************/

void
mf_papi_shutdown()
{
    int core;

    for (core = 0; core != maximum_number_of_cores; ++core) {
        int retval = PAPI_stop(event_sets[core], NULL);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("Couldn't stop PAPI EventSet: %s", error);
        }

        retval = PAPI_cleanup_eventset(event_sets[core]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("Couldn't cleanup PAPI EventSet: %s", error);
        }

        retval = PAPI_destroy_eventset(&event_sets[core]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("Couldn't destroy PAPI EventSet: %s", error);
        }
    }

    PAPI_shutdown();
}