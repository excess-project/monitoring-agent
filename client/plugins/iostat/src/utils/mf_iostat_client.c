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

#include <malloc.h>
#include <ctype.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "mf_iostat_connector.h"
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
    ++argv;
    mf_iostat_init(argv, --argc);
    do {
        Iostat_Plugin *iostat = malloc(sizeof(Iostat_Plugin));
        mf_iostat_read(iostat);
        printf("%s", to_csv(iostat));
        free(iostat);
        sleep(2);
    } while (1);

    free(csv);
}

static void
my_exit_handler(int s)
{
    puts("\nBye bye!");
    exit(EXIT_FAILURE);
}

static char*
to_csv(Iostat_Plugin *iostat)
{
    int i;
    char *row;

    if (iostat == NULL) {
        log_error("No data fetched during profiling: %s", "NULL");
        return NULL;
    }

    memset(csv, 0, 4096 * sizeof(char));
    row = malloc(256 * sizeof(char));
    for (i = 0; i < iostat->num_events; ++i) {
        sprintf(row, "\"%s\",%s\n", iostat->events[i], iostat->values[i]);
        strcat(csv, row);
    }
    free(row);

    return csv;
}
