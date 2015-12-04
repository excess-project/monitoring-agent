/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h> /* nanosleep */
#include <stdlib.h> /* malloc, exit, free, ... */

/* monitoring-related includes */
#include "mf_debug.h" /* log_warn, log_info, ... */
#include "mf_rapl_connector.h" /* get_available_events */

static char *csv;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static char* to_csv();
static void my_exit_handler();

/*******************************************************************************
 * Main
 ******************************************************************************/

int
main(int argc, char** argv)
{
    csv = malloc(4096 * sizeof(char));

    if (argc <= 1) {
        log_warn("No events given to measure: %d", argc);
        exit(EXIT_FAILURE);
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    /***************************************************************************
     * Monitoring
     **************************************************************************/

    struct timespec profile_time = { 0, 0 };
    profile_time.tv_sec = 0;
    profile_time.tv_nsec = 500000000;

    ++argv;
    --argc;

    /*
     * initialize RAPL plugin
     */
    RAPL_Plugin *monitoring_data = malloc(sizeof(RAPL_Plugin));
    mf_rapl_init(monitoring_data, argv, argc);

    do {
        /*
         * sampling
         */
        mf_rapl_sample(monitoring_data);

        puts(to_csv(monitoring_data));

        /*
         * sleep for a given time until next sample
         */
        nanosleep(&profile_time, NULL);
    } while (1);

    free(csv);
}

/*******************************************************************************
 * my_exit_handler
 ******************************************************************************/

static void
my_exit_handler(int s)
{
    mf_rapl_shutdown();
    puts("\nBye bye!");
    exit(EXIT_FAILURE);
}

/*******************************************************************************
 * to_csv
 ******************************************************************************/

static char*
to_csv(RAPL_Plugin *rapl)
{
    int i;
    char *row;

    if (rapl == NULL) {
        log_error("No data fetched during profiling: %s", "NULL");
        return NULL;
    }

    memset(csv, 0, 4096 * sizeof(char));
    row = malloc(256 * sizeof(char));
    for (i = 0; i < rapl->num_events; ++i) {
        sprintf(row, "\"%s\",%.4f\n", rapl->events[i], rapl->values[i]);
        strcat(csv, row);
    }
    free(row);

    return csv;
}