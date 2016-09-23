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

/** @file mf_acme_client.c
 *  @brief Client that demonstrates the usage of the ACME plug-in.
 *
 *  @author Fangli Pi
 */

#include <pthread.h> /* nanosleep */
#include <stdlib.h> /* malloc, exit, free, ... */
#include <signal.h>
#include <math.h>


/* monitoring-related includes */
#include "mf_debug.h" /* log_warn, log_info, ... */
#include "mf_acme_connector.h" /* get_available_events */

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static int app_running = 1;
static void my_exit_handler();


/* Test of the acme plugin */
int
main(int argc, char** argv)
{
    if (argc <= 1) {
        log_warn("No events given to measure: %d", argc);
        exit(EXIT_FAILURE);
    }

    struct sigaction sig;
    sigaction(SIGINT, NULL, &sig);
    sig.sa_handler = my_exit_handler;
    sigaction(SIGINT, &sig, NULL);
    
    struct timespec profile_time = { 0, 0 };
    profile_time.tv_sec = 0;
    profile_time.tv_nsec = 500000000;

    ++argv;
    --argc;

    /*
     * initialize acme plugin
     */
    ACME_Plugin *monitoring_data = malloc(sizeof(ACME_Plugin));
    mf_acme_init(monitoring_data, argv, argc);

    while(app_running){
        /*
         * sleep for a given time until next sample
         */
        nanosleep(&profile_time, NULL);
        /*
         * sampling
         */
        mf_acme_sample(monitoring_data);
        puts(mf_acme_to_json(monitoring_data));
    }
    mf_acme_shutdown();
    return 0;
}

/* Exit handler */
static void
my_exit_handler(int s)
{
    app_running = 0;
    puts("\nBye bye!");
    exit(0);
}
