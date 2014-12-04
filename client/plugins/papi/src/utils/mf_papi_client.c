#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <malloc.h>
#include <papi.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static int mf_set_affinity(int);
int mf_PAPI_create_eventset_systemwide(int*, int, int, int, int);


int
main(int argc, char** argv)
{
    int retval;
    int sleep_time;
    int i, num_events;
    int cdix, cpu_num;
    int domain, granularity; 
    int EventSet = PAPI_NULL;
    long long *values;

    /***************************************************************************
     * Set default values
     **************************************************************************/

    cdix = 0;
    cpu_num = 0;
    domain = PAPI_DOM_ALL;
    granularity = PAPI_GRN_SYS;
    sleep_time = 1;

    /***************************************************************************
     * Initialize PAPI library
     **************************************************************************/

    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        log_error("main(int, char**) - PAPI_library_init: %s", PAPI_strerror(retval));
    }

    /***************************************************************************
     * Create EventSet
     **************************************************************************/

    retval = mf_PAPI_create_eventset_systemwide(
        &EventSet,
        cdix,
        cpu_num,
        domain,
        granularity
    );
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_add_event: %s", PAPI_strerror(retval));
    }

    /***************************************************************************
     * Add events passed via command line
     **************************************************************************/

    for (num_events = 0, i = 1; i != argc; ++i) {
        retval = PAPI_add_named_event(EventSet, argv[i]);
        if (retval != PAPI_OK) {
            log_error("main(int, char**) - PAPI_add_named_event (%s): %s",
                argv[i], PAPI_strerror(retval));
        } else {
            ++num_events;
            log_info("main(int, char**) - Added event %s", argv[i]);
        } 
    }

    if (num_events == 0) {
        log_warn("main(int, char**) - No events added for monitoring. %s", "Abort.");
        PAPI_shutdown();
        exit(1);
    }

    values = malloc(num_events * sizeof(long long));

    /***************************************************************************
     * Start PAPI
     **************************************************************************/
 
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_start: %s", PAPI_strerror(retval));
    }

    sleep(sleep_time);

    /***************************************************************************
     * Stop PAPI
     **************************************************************************/

    retval = PAPI_stop(EventSet, values);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_stop: %s", PAPI_strerror(retval));
    }

    /***************************************************************************
     * Print Counters
     **************************************************************************/
    
    for (i = 0; i < num_events; i++) {
        printf("CPU%d \t %s \t%lld\n", cpu_num, argv[i + 1], values[i]);
    }

    free(values);
    PAPI_shutdown();
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

int
mf_PAPI_create_eventset_systemwide(
    int *EventSet,
    int cidx,
    int cpu_num,
    int domain,
    int granularity)
{
    int retval;
    PAPI_domain_option_t domain_opt;
    PAPI_granularity_option_t gran_opt;
    PAPI_cpu_option_t cpu_opt;

    retval = PAPI_create_eventset(EventSet);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_create_eventset: %s",
            PAPI_strerror(retval));
        return retval;
    }

    retval = PAPI_assign_eventset_component(*EventSet, cidx);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_assign_eventset_component: (%s)", 
            PAPI_strerror(retval));
        return retval;
    }

    domain_opt.def_cidx = cidx;
    domain_opt.eventset = *EventSet;
    domain_opt.domain = domain;
    retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*) &domain_opt);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_set_opt (PAPI_DOMAIN): %s",
            PAPI_strerror(retval));
        return retval;
    }

    gran_opt.eventset = *EventSet;
    gran_opt.granularity = granularity;
    retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*) &gran_opt);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_set_opt (PAPI_GRANUL): %s",
            PAPI_strerror(retval));
        return retval;
    }

    cpu_opt.eventset = *EventSet;
    cpu_opt.cpu_num = cpu_num;
    retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*) &cpu_opt);
    if (retval != PAPI_OK) {
        log_error("main(int, char**) - PAPI_set_opt (PAPI_CPU_ATTACH): %s",
            PAPI_strerror(retval));
        return retval;
    }

    retval = mf_set_affinity(cpu_num);
    if (retval != PAPI_OK) {
        log_error("mf_PAPI_create_eventset_systemwide() - sched_setaffinity: %s",
            PAPI_strerror(retval));
        return retval;
    }

    return retval;
}