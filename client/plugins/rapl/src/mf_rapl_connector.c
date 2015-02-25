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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mf_debug.h"
#include "mf_rapl_connector.h"

void
initialize_PAPI()
{
    if (PAPI_is_initialized()) {
        return;
    }

    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("RAPL:: - PAPI_library_init: %s", error);
    }
}


int
get_available_events(RAPL_Plugin *rapl, struct timespec profile_interval)
{
    long long before_time, after_time;
    double elapsed_time;
    int EventSet = PAPI_NULL;
    int num_events = 0;

    initialize_PAPI();

    int retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    int code = PAPI_NATIVE_MASK;
    int rapl_cid = get_rapl_component_id();
    int r = PAPI_enum_cmp_event(&code, PAPI_ENUM_FIRST, rapl_cid);
    PAPI_event_info_t evinfo;

    while (r == PAPI_OK) {
        retval = PAPI_event_code_to_name(code, rapl->events[num_events]);
        if (retval != PAPI_OK) {
            printf("Error translating %#x\n", code);
            return -1;
        }

        retval = PAPI_get_event_info(code, &evinfo);
        if (retval != PAPI_OK) {
            printf("Error getting event info: %d\n", retval);
            return -1;
        }

        rapl->data_types[num_events] = evinfo.data_type;

        retval = PAPI_add_event(EventSet, code);
        if (retval != PAPI_OK) {
            break;
        }
        num_events++;

        r = PAPI_enum_cmp_event(&code, PAPI_ENUM_EVENTS, rapl_cid);
    }

    rapl->values = calloc(num_events, sizeof(long long));
    long long *values = calloc(num_events, sizeof(long long));
    if (values == NULL) {
        return -1;
    }
    rapl->num_events = num_events;

    before_time = PAPI_get_real_nsec();
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    nanosleep(&profile_interval, NULL);

    after_time = PAPI_get_real_nsec();
    retval = PAPI_stop(EventSet, values);
    if (retval != PAPI_OK) {
        return -1;
    }

    elapsed_time = ((double)(after_time - before_time)) / 1.0e9;

    int i;
    for (i = 0; i < num_events; ++i) { // average
        rapl->values[i] = ((double) values[i] / 1.0e9) / elapsed_time;
    }

    retval = PAPI_cleanup_eventset(EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    retval = PAPI_destroy_eventset(&EventSet);
    if (retval != PAPI_OK) {
        return -1;
    }

    return num_events;
}


int
get_rapl_component_id()
{
    int cid;
    int numcmp;
    int rapl_cid;
    const PAPI_component_info_t *cmpinfo = NULL;

    initialize_PAPI();
    numcmp = PAPI_num_components();
    for (cid = 0; cid < numcmp; ++cid) {
        if ((cmpinfo = PAPI_get_component_info(cid)) == NULL) {
            printf("%s", "RAPL: cannot call component info");
            return 0;
        }

        if (strstr(cmpinfo->name, "rapl")) {
            rapl_cid = cid;

            if (cmpinfo->disabled) {
                printf("%s", "RAPL: component is disabled");
                return 0;
            }
            break;
        }
    }

    if (cid == numcmp) {
        printf("%s", "RAPL: component not found");
        return 0;
    }

    return rapl_cid;
}
