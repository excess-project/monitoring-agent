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

#define LEN 64

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static int is_valid_number(char*);
static int mf_set_affinity(int);
int mf_PAPI_create_eventset_systemwide(int*, int, int, int, int);

/*******************************************************************************
 * Main
 ******************************************************************************/

int
main(int argc, char** argv)
{
    long long *values;
    char error_val[LEN];
    int retval, ncpu, nctr, cdix, cpu_num, i;
    int EventSet = PAPI_NULL;
    char event_name[256];
    int domain, granularity; 
    int sleep_time;

    /***************************************************************************
     * Set default values
     **************************************************************************/

    cdix = 0;
    cpu_num = 0;
    domain = PAPI_DOM_ALL;
    granularity = PAPI_GRN_SYS;
    strcpy(event_name, "PAPI_L2_DCA");
    sleep_time = 1;

    /***************************************************************************
     * Read argument passed by the command line
     **************************************************************************/

    if (argc > 1 && strlen(argv[1]) > 0) {
        strcpy(event_name, argv[1]);
    }
    log_info("main(int, char**) - using %s as counter", event_name);

    if (argc > 2 && is_valid_number(argv[2])) {
        sleep_time = atoi(argv[2]);
    }
    log_info("main(int, char**) - counting for %d seconds", sleep_time);

    /***************************************************************************
     * Initialize PAPI library
     **************************************************************************/

    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_library_init: %s", error_val);
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
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_add_event: %s", error_val);
    }

    /***************************************************************************
     * Output initialization
     **************************************************************************/

    ncpu = PAPI_get_opt(PAPI_MAX_CPUS, NULL);
    nctr = PAPI_get_opt(PAPI_MAX_HWCTRS, NULL);
    values = (long long *) malloc(ncpu * nctr * sizeof(long long));
    memset(values, 0x0, (ncpu * nctr * sizeof (long long)));

    /***************************************************************************
     * Add event
     **************************************************************************/

    retval = PAPI_add_named_event(EventSet, event_name);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_add_event: %s", error_val);
    }

    /***************************************************************************
     * Start PAPI
     **************************************************************************/
 
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_start: %s", error_val);
    }

    sleep(sleep_time);

    /***************************************************************************
     * Stop PAPI
     **************************************************************************/

    retval = PAPI_stop(EventSet, values);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_stop: %s", error_val);
    }

    /***************************************************************************
     * Print Counters
     **************************************************************************/
    
    for (i = 0; i < ncpu; i++) {
        printf("CPU%d \t %s \t%lld\n", i, event_name, values[0 + i * nctr]);
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

static int
is_valid_number(char* string)
{
    int i;

    for(i = 0; i < strlen(string); i++) {
        if (isdigit(string[i]) == 0) break;
    }

    if (i != strlen(string)) {
        log_error("is_valid_number(char*) - %s is no valid integer", string);
        return 0;
    }

    return 1;
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
    char error_val[LEN];
    PAPI_domain_option_t domain_opt;
    PAPI_granularity_option_t gran_opt;
    PAPI_cpu_option_t cpu_opt;

    retval = mf_set_affinity(cpu_num);
    if (retval != PAPI_OK) {
        log_error("mf_PAPI_create_eventset_systemwide() - sched_setaffinity: \
            %s", strerror(errno));
        return retval;
    }

    retval = PAPI_create_eventset(EventSet);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_create_eventset: %s", error_val);
        return retval;
    }

    /***************************************************************************
     * Sets some flags for per node usage
     **************************************************************************/

    retval = PAPI_assign_eventset_component(*EventSet, cidx);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_assign_eventset_component: \
            (%s)", error_val);
        return retval;
    }

    domain_opt.def_cidx = cidx;
    domain_opt.eventset = *EventSet;
    domain_opt.domain = domain;
    retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*) &domain_opt);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_set_opt (PAPI_DOMAIN): \
            %s", error_val);
        return retval;
    }

    gran_opt.eventset = *EventSet;
    gran_opt.granularity = granularity;
    retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*) &gran_opt);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_set_opt (PAPI_GRANUL): \
            %s", error_val);
        return retval;
    }

    cpu_opt.eventset = *EventSet;
    cpu_opt.cpu_num = cpu_num;
    retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*) &cpu_opt);
    if (retval != PAPI_OK) {
        snprintf(error_val, LEN, "%d", retval);
        log_error("main(int, char**) - PAPI_set_opt (PAPI_CPU_ATTACH): \
            %s", error_val);
        return retval;
    }

    return retval;
}