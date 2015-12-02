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
#include <time.h>
#include <unistd.h>

#include "mf_infiniband_connector.h"
#include "mf_debug.h"

static char *csv;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static char* to_csv();
static void my_exit_handler();

int
main(int argc, char** argv)
{
    INFINIBAND_Plugin *infiniband = malloc(sizeof(INFINIBAND_Plugin));
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
    mf_infiniband_init(argv, --argc);
    do {
        mf_infiniband_profile(profile_time);
        mf_infiniband_read(infiniband, argv);
        puts(to_csv(infiniband));
    } while (1);

    free(csv);
}

static void
my_exit_handler(int s)
{
    mf_infiniband_shutdown();
    puts("\nBye bye!");
    exit(EXIT_FAILURE);
}

static char*
to_csv(INFINIBAND_Plugin *infiniband)
{
    int i;
    char *row;

    if (infiniband == NULL) {
        log_error("No data fetched during profiling: %s", "NULL");
        return NULL;
    }

    memset(csv, 0, 4096 * sizeof(char));
    row = malloc(256 * sizeof(char));
    for (i = 0; i < infiniband->num_events; ++i) {
        sprintf(row, "\"%s\",%lld\n", infiniband->events[i], infiniband->values[i]);
        strcat(csv, row);
    }
    free(row);

    return csv;
}
