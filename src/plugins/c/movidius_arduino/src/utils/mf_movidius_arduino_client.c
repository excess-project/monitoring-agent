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

/** @file mf_movidius_arduino_client.c
 *  @brief Client that demonstrates the usage of the MOVIDIUS_ARDUINO plug-in.
 *
 *  @author Dmitry Khabi
 */

#include <pthread.h> /* nanosleep */
#include <stdlib.h> /* malloc, exit, free, ... */
/* monitoring-related includes */
#include "mf_debug.h" /* log_warn, log_info, ... */
#include "mf_movidius_arduino_connector.h" /* get_available_events */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static void my_exit_handler();

/*******************************************************************************
 * Main
 ******************************************************************************/

int
main(int argc, char** argv)
{
    
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    /***************************************************************************
     * Monitoring
     **************************************************************************/

    struct timespec profile_time = { 0, 0 };
    profile_time.tv_sec = 1;
    profile_time.tv_nsec = 0;
    size_t num_cores = 1;

    ++argv;
    --argc;

    /*
     * initialize PAPI plugin
     */
    MOVI_Plugin* monitoring_data = malloc(num_cores * sizeof(MOVI_Plugin));
    mf_movi_init(monitoring_data, argv, argc);

    do {
        /*
         * sampling
         */
        mf_movi_sample(monitoring_data);

        /*
         * print current values
         */
        puts(mf_movi_to_json(monitoring_data));

        /*
         * sleep for a given time until next sample
         */
        nanosleep(&profile_time, NULL);
    } while (1);
}

/*******************************************************************************
 * my_exit_handler
 ******************************************************************************/

static void
my_exit_handler(int s)
{
    mf_movi_shutdown();
    puts("\nBye bye!");
    exit(EXIT_SUCCESS);
}
