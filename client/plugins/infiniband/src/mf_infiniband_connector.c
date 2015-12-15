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

#include <stdlib.h> /* malloc */

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_infiniband_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

/*
 * declares if the plug-in (i.e., INFINIBAND) is already initialized
 */
static int is_initialized = 0;

/*
 * declares if the INFINIBAND component is enabled to be used for monitoring
 *
 * states: (-1) not initialized, (0) disabled, (1) enabled
 */
static int is_available = -1;

int EventSet = PAPI_NULL;
long long before_time, after_time;
long long *values;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static int is_infiniband_initialized();
static int enable_papi_library();

/*******************************************************************************
 * mf_infiniband_is_enabled
 ******************************************************************************/

int
mf_infiniband_is_enabled()
{
    int numcmp, cid;
    const PAPI_component_info_t *cmpinfo = NULL;
    enable_papi_library();

    if (is_available > -1) {
        return is_available;
    }

    numcmp = PAPI_num_components();
    for (cid = 0; cid < numcmp; cid++) {
        cmpinfo = PAPI_get_component_info(cid);
        if (strstr(cmpinfo->name, "infiniband")) {
            if (cmpinfo->disabled) {
                is_available = FAILURE;
                log_warn("Component is DISABLED for this CPU (%d)", cid);
                return FAILURE;
            } else {
                is_available = SUCCESS;
                log_info("Component is ENABLED (%s)", cmpinfo->name);
            }
            return is_available;
        }
    }

    is_available = FAILURE;
    return is_available;
}

/*******************************************************************************
 * mf_infiniband_init
 ******************************************************************************/

int
mf_infiniband_init(INFINIBAND_Plugin *data, char **rapl_events, size_t num_events)
{
    /*
     * setup PAPI library
     */
    if (enable_papi_library() != SUCCESS) {
        return FAILURE;
    }

    /*
     * create PAPI EventSet
     */
    int retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        log_error("ERROR: Couldn't create EventSet %s", PAPI_strerror(retval));
        return FAILURE;
    }

    /*
     * add user-defined metrics to the EventSet
     */
    int idx, registered_idx = 0;
    for (idx = 0; idx != num_events; ++idx) {
        retval = PAPI_add_named_event(EventSet, rapl_events[idx]);
        if (retval != PAPI_OK) {
            char *err = PAPI_strerror(retval);
            log_warn("Couldn't add PAPI event (%s): %s", rapl_events[idx], err);
        } else {
            log_info("Added PAPI event %s", rapl_events[idx]);

            /*
             * register added PAPI event at the internal data structure
             */
            data->events[registered_idx] = malloc(PAPI_MAX_STR_LEN + 1);
            strcpy(data->events[registered_idx], rapl_events[idx]);
            registered_idx = registered_idx + 1;
        }
    }
    data->num_events = registered_idx;
    values = calloc(registered_idx, sizeof(long long));

    /*
     * start monitoring registered events
     */
    before_time = PAPI_get_real_nsec();
    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        log_error("ERROR: Couldn't start monitoring %s", PAPI_strerror(retval));
        return FAILURE;
    }

    return registered_idx;
}

/*******************************************************************************
 * mf_infiniband_sample
 ******************************************************************************/

int
mf_infiniband_sample(INFINIBAND_Plugin *data)
{
    /*
     * initialize array to store monitoring results
     */
    size_t size = data->num_events;
    if (values == NULL) {
        log_error("Couldn't initialize long long values %s", "NULL");
        return FAILURE;
    }

    /*
     * read measurements
     */
    after_time = PAPI_get_real_nsec();
    int retval = PAPI_read(EventSet, values);
    if (retval != PAPI_OK) {
        return FAILURE;
    }

    /*
     * account for time passed between last measurement and now
     */
    int idx;
    double elapsed_time = ((double) (after_time - before_time)) / 1.0e9;
    for (idx = 0; idx < size; ++idx) {
        data->values[idx] = ((double) values[idx]) / elapsed_time;
    }

    /*
     * update time interval
     */
    before_time = after_time;

    /*
     * reset counters to zero
     */
    PAPI_reset(EventSet);

    return SUCCESS;
}

/*******************************************************************************
 * mf_infiniband_to_json
 ******************************************************************************/

char*
mf_infiniband_to_json(INFINIBAND_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"infiniband\"");

    int idx;
    size_t size = data->num_events;
    for (idx = 0; idx < size; ++idx) {
        sprintf(metric, ",\"%s\":%lld", data->events[idx], data->values[idx]);
        strcat(json, metric);
    }
    free(metric);

    return json;
}

/*******************************************************************************
 * enable_papi_library
 ******************************************************************************/

static int
enable_papi_library()
{
    if (is_infiniband_initialized()) {
        return is_initialized;
    }

    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        char *error = PAPI_strerror(retval);
        log_error("ERROR while initializing PAPI library: %s", error);
        is_initialized = FAILURE;
    } else {
        is_initialized = SUCCESS;
    }

    return is_initialized;
}

/*******************************************************************************
 * is_infiniband_initialized
 ******************************************************************************/

static int
is_infiniband_initialized()
{
    return is_initialized;
}

/*******************************************************************************
 * mf_infiniband_shutdown
 ******************************************************************************/

void
mf_infiniband_shutdown()
{
    int retval = PAPI_stop(EventSet, NULL);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("Couldn't stop PAPI EventSet: %s", error);
    }

    retval = PAPI_cleanup_eventset(EventSet);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("Couldn't cleanup PAPI EventSet: %s", error);
    }

    retval = PAPI_destroy_eventset(&EventSet);
    if (retval != PAPI_OK) {
        char *error = PAPI_strerror(retval);
        log_error("Couldn't destroy PAPI EventSet: %s", error);
    }

    PAPI_shutdown();
}
