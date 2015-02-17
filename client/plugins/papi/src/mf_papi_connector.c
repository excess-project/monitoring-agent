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

static int cidx;
static int *event_sets;
static int num_cores;
static int domain;
static int granularity;
static int *num_events_per_socket;
static long long **values_per_core;
static int is_initialized;

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

void
mf_papi_init(char **named_events, size_t num_events, int requested_num_cores)
{
    if (is_papi_initialized()) {
        return;
    }

    cidx = 0;
    num_cores = 0;
    event_sets = NULL;
    domain = PAPI_DOM_ALL;
    granularity = PAPI_GRN_SYS;
    num_events_per_socket = NULL;
    values_per_core = NULL;

    load_papi();
    get_max_cpus(&num_cores);
    if (requested_num_cores < num_cores) {
        if (requested_num_cores > 0) {
            num_cores = requested_num_cores;
        }
    }

    create_eventset_for_each_core();
    bind_events_to_all_cores(named_events, num_events);
    #ifdef DEBUG
    check_events();
    #endif

    is_initialized = 1;
}

static int
is_papi_initialized()
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
mf_papi_shutdown()
{
    PAPI_shutdown();
}

static void
create_eventset_for_each_core()
{
    init_eventsets();

    for (int cpu_num = 0; cpu_num != num_cores; ++cpu_num) {
        (void) create_eventset_systemwide(event_sets+cpu_num, cpu_num);
    }
}

static void
get_max_cpus(int *max_cpus)
{
    *max_cpus = PAPI_get_opt(PAPI_MAX_CPUS,/*@-nullpass@*/ NULL);

    if (*max_cpus <= 0) {
        char itostr[*max_cpus];
        (void) snprintf(itostr, sizeof(itostr) / sizeof(char), "%d", *max_cpus);
        log_error("get_max_cpus() - PAPI_get_opt(PAPI_MAX_CPUS): %s", itostr);
        return;
    }

    log_info("get_max_cpus() - max_cpus = %d", *max_cpus);
}

static void
init_eventsets()
{
    event_sets = malloc(num_cores * sizeof(int));
    if (event_sets == NULL) {
        log_error("init_event_sets() - Could not allocate memory for %s", "event_sets");
        return;
    }

    for (int i = 0; i != num_cores; ++i) {
        event_sets[i] = PAPI_NULL;
    }
}

static int
create_eventset_systemwide(int *EventSet, int cpu_num)
{
    int retval;
    PAPI_domain_option_t domain_opt;
    PAPI_granularity_option_t gran_opt;
    PAPI_cpu_option_t cpu_opt;

    retval = PAPI_create_eventset(EventSet);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_create_eventset: %s", error);
        return retval;
    }

    retval = PAPI_assign_eventset_component(*EventSet, cidx);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_assign_eventset_component: (%s)", error);
        return retval;
    }

    domain_opt.def_cidx = cidx;
    domain_opt.eventset = *EventSet;
    domain_opt.domain = domain;
    retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*) &domain_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_set_opt (PAPI_DOMAIN): %s", error);
        return retval;
    }

    gran_opt.eventset = *EventSet;
    gran_opt.granularity = granularity;
    retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*) &gran_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_set_opt (PAPI_GRANUL): %s", error);
        return retval;
    }

    cpu_opt.eventset = *EventSet;
    cpu_opt.cpu_num = cpu_num;
    retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*) &cpu_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_set_opt (PAPI_CPU_ATTACH): %s", error);
        PAPI_shutdown();
        exit(EXIT_FAILURE);
    }

    retval = mf_set_affinity(cpu_num);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - sched_setaffinity: %s", error);
        return retval;
    }

    debug("EventSet created for CPU%d", cpu_num);

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