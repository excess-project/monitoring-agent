/*
 * Copyright (C) 2016 University of Stuttgart
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

/** @file mf_sensors_client.c
 *  @brief Client that demonstrates the usage of the sensors plug-in.
 *
 *  @author Dennis Hoppe (hopped)
 */

#include <signal.h>  /* sigIntHandler */
#include <pthread.h> /* nanosleep */
#include <stdlib.h>  /* malloc, exit, free, ... */

/* monitoring-related includes */
#include "mf_debug.h" /* log_warn, log_info, ... */
#include "mf_sensors_connector.h" /* get_available_events */

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static void my_exit_handler();

/* Test of the sensors plugin */
int
main(int argc, char** argv)
{
    if (argc <= 1) {
        log_warn("No events given to measure: %d", argc);
        exit(EXIT_FAILURE);
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    struct timespec profile_time = { 0, 0 };
    profile_time.tv_sec = 0;
    profile_time.tv_nsec = 500000000;

    ++argv;
    --argc;

    /*
     * initialize sensors plugin
     */
    SENSORS_Plugin *monitoring_data = malloc(sizeof(SENSORS_Plugin));
    mf_sensors_init(monitoring_data, argv, argc);

    do {
        /*
         * sampling
         */
        mf_sensors_sample(monitoring_data);
        puts(mf_sensors_to_json(monitoring_data));

        /*
         * sleep for a given time until next sample
         */
        nanosleep(&profile_time, NULL);
    } while (1);
}

/* Exit handler */
static void
my_exit_handler(int s)
{
    mf_sensors_shutdown();
    puts("\nBye bye!");
    exit(EXIT_SUCCESS);
}