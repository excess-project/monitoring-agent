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

#include "debug.h"

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

int mf_PAPI_create_eventset_systemwide(int*, int, int, int, int);
static void start_monitoring(int*, int*, long long**, int, char**);
static int mf_set_affinity(int);
static void my_exit_handler(int);

int
main(int argc, char** argv)
{
    int cidx;
    int retval;
    int *event_sets;
    int domain, granularity;
    int i, j, cpu_num, num_cores;
    int *num_events_per_socket;
    long long **values_per_core;

    /***************************************************************************
     * Set default values
     **************************************************************************/

    cidx = 0;
    domain = PAPI_DOM_ALL;
    granularity = PAPI_GRN_SYS;

    /***************************************************************************
     * Initialize PAPI library
     **************************************************************************/

    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("main(int, char**) - PAPI_library_init: %s", error);
        free(error);
    }

    /***************************************************************************
     * Create EventSets for each socket
     **************************************************************************/

    num_cores = PAPI_get_opt(PAPI_MAX_CPUS,/*@-nullpass@*/ NULL);
    log_info("main(int, char**) - num_cores = %d", num_cores);
    if (num_cores <= 0) {
        char itostr[num_cores];
        (void) snprintf(itostr, sizeof(itostr) / sizeof(char), "%d", num_cores);
        log_error("main(int, char**) - PAPI_get_opt(PAPI_MAX_CPUS): %s", itostr);
        PAPI_shutdown();
        exit(EXIT_FAILURE);
    }

    event_sets = malloc(num_cores * sizeof(int));
    if (event_sets == NULL) {
        log_error("Could not allocated memory: %s", "event_sets");
        PAPI_shutdown();
        exit(EXIT_FAILURE);
    }
    for (i = 0; i != num_cores; ++i) {
        event_sets[i] = PAPI_NULL;
    }

    for (cpu_num = 0; cpu_num != num_cores; ++cpu_num) {
        retval = mf_PAPI_create_eventset_systemwide(
            event_sets+cpu_num, cidx, cpu_num, domain, granularity
        );
    }

    /***************************************************************************
     * Add events passed via command line to each socket separately
     **************************************************************************/

    num_events_per_socket = malloc(num_cores * sizeof(int));
    for (i = 0; i != num_cores; ++i) {
        num_events_per_socket[i] = 0;
        for (j = 1; j != argc; ++j) {
            retval = PAPI_add_named_event(event_sets[i], argv[j]);
            if (retval != PAPI_OK) {
                char *error = PAPI_strerror(retval);
                log_error("main(int, char**) - PAPI_add_named_event (%s): %s", argv[j], error);
                free(error);
            } else {
                num_events_per_socket[i]++;
                log_info("main(int, char**) - Added event %s to CPU%d", argv[j], i);
            }
        }
    }

    for (i = 0; i != num_cores; ++i) {
        debug("num_events_per_socket: %d", num_events_per_socket[i]);
        if (num_events_per_socket[i] == 0) {
            log_warn("main(int, char**) - No events added for monitoring. %s", "Abort.");
            free(event_sets);
            PAPI_shutdown();
            exit(EXIT_FAILURE);
        }
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    values_per_core = malloc(num_cores * sizeof(long long *));

    do {
        start_monitoring(
            event_sets,
            num_events_per_socket,
            values_per_core,
            num_cores,
            argv
        );
    } while (1);

    free(num_events_per_socket);
    free(values_per_core);

    for (i = 0; i != num_cores; ++i) {
        retval = PAPI_cleanup_eventset(event_sets[i]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("main(int, char**) - PAPI_cleanup_eventset: %s", error);
            free(error);
        }
        retval = PAPI_destroy_eventset(event_sets+i);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("main(int, char**) - PAPI_destroy_eventset: %s", error);
            free(error);
        }
    }

    debug("suhtdown %s", "bla");
    PAPI_shutdown();
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
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_create_eventset: %s", error);
        free(error);
        return retval;
    }

    retval = PAPI_assign_eventset_component(*EventSet, cidx);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_assign_eventset_component: (%s)", error);
        free(error);
        return retval;
    }

    domain_opt.def_cidx = cidx;
    domain_opt.eventset = *EventSet;
    domain_opt.domain = domain;
    retval = PAPI_set_opt(PAPI_DOMAIN, (PAPI_option_t*) &domain_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_set_opt (PAPI_DOMAIN): %s", error);
        free(error);
        return retval;
    }

    gran_opt.eventset = *EventSet;
    gran_opt.granularity = granularity;
    retval = PAPI_set_opt(PAPI_GRANUL, (PAPI_option_t*) &gran_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_set_opt (PAPI_GRANUL): %s", error);
        free(error);
        return retval;
    }

    cpu_opt.eventset = *EventSet;
    cpu_opt.cpu_num = cpu_num;
    retval = PAPI_set_opt(PAPI_CPU_ATTACH, (PAPI_option_t*) &cpu_opt);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - PAPI_set_opt (PAPI_CPU_ATTACH): %s", error);
        free(error);
        return retval;
    }

    retval = mf_set_affinity(cpu_num);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("create_eventset_systemwide - sched_setaffinity: %s", error);
        free(error);
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
start_monitoring(
    int *event_sets,
    int *num_events_per_socket,
    long long **values_per_core,
    int num_cores,
    char **argv)
{
    int i, j;
    int retval;

    /***************************************************************************
     * Start PAPI
     **************************************************************************/

    for (i = 0; i != num_cores; ++i) {
        debug("Start PAPI Monitoring for CPU%d", i);
        retval = PAPI_start(event_sets[i]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("main(int, char**) - PAPI_start: %s", error);
            free(error);
        }
    }

    sleep(1);

    /***************************************************************************
     * Stop PAPI
     **************************************************************************/

    for (i = 0; i != num_cores; ++i) {
        debug("Stop PAPI Monitoring for CPU%d", i);
        values_per_core[i] = malloc(num_events_per_socket[i] * sizeof(long long));
        retval = PAPI_stop(event_sets[i], values_per_core[i]);
        if (retval != PAPI_OK) {
            char *error = PAPI_strerror(retval);
            log_error("main(int, char**) - PAPI_stop: %s", error);
            free(error);
        }
    }

    /***************************************************************************
     * Print Counters
     **************************************************************************/

    for (i = 0; i != num_cores; ++i) {
        for (j = 0; j != num_events_per_socket[i]; ++j) {
            printf("CPU%d \t %s \t\t%lld\n", i, argv[j+1], values_per_core[i][j]);
        }
        puts("--------------------------------------------");
    }
}

static void
my_exit_handler(int s)
{
    puts("\nBye bye!");
    exit(EXIT_FAILURE);
}