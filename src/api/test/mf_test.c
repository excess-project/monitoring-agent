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

/**
 * @brief Example showing sending user-defined metrics to the MF server
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mf_api.h>

int
main(int argc, char** argv)
{
    int i, c;
    const char* URL = NULL;
    long double start_time, end_time;

    /***************************************************************************
     * GET OPTIONS
     **************************************************************************/

    opterr = 0;
    while ((c = getopt(argc, argv, "?k:u:")) != -1) {
        switch (c) {
            case 'u':
                URL = optarg;
                break;
            case '?':
                if (optopt == 'u') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                opterr = 1;
                return(EXIT_FAILURE);
            default:
                fprintf(stderr, "Please specify some options");
                opterr = 1;
                abort();
        }
    }

    /***************************************************************************
     * Initialize API and send custom data to the server
     **************************************************************************/

    mf_api_initialize(URL, NULL);

    char* EXE_ID = mf_api_get_execution_id();

    printf("EXE_ID generated is :%s.\n", EXE_ID);

    mf_api_initialize(URL, EXE_ID);

    //mf_api_initialize(URL);
    start_time = mf_api_start_profiling("fcnt1");

    char* mem_info = malloc(256 * sizeof(char));
    for (i = 0; i != 2; ++i) {
        sprintf(mem_info, "\"mem_used\":%d", i);
        mf_api_send(mem_info);

        usleep(250000);
    }
    free(mem_info);

    end_time = mf_api_stop_profiling("fcnt1");

    usleep(1000000);

    /***************************************************************************
     * Retrieve information from the database
     **************************************************************************/

    char *response = malloc(10000 * sizeof(char));
    get_data_by_interval(start_time, end_time, response);
    puts(response);

    return EXIT_SUCCESS;
}
