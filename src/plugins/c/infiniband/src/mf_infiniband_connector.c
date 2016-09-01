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
#include "mf_types.h"
#include "publisher.h"
#include "mf_infiniband_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

/* Declares if the plug-in (i.e., INFINIBAND) is already initialized */
static int is_initialized = 0;

/* Declares if the INFINIBAND component is enabled to be used for monitoring
 * states: (-1) not initialized, (0) disabled, (1) enabled */
static int is_available = -1;

int EventSet = PAPI_NULL;
long long before_time, after_time;
long long *values;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static int mf_infiniband_unit_init(metric_units *unit, int infiniband_cid);
static int is_infiniband_initialized();
static int enable_papi_library();

/* Checks if Infiniband component of the PAPI library is enabled
 * return 1 if component is enabled; 0 otherwise. */
int
mf_infiniband_is_enabled()
{
    int numcmp, cid;
    const PAPI_component_info_t *cmpinfo = NULL;
    metric_units *INFINIBAND_units = malloc(sizeof(metric_units));
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
            /* init infiniband all metric names and units */
            mf_infiniband_unit_init(INFINIBAND_units, cid);
            /* publish the units to mf_server */
            publish_unit(INFINIBAND_units);
            return is_available;
        }
    }

    is_available = FAILURE;
    return is_available;
}

/* Initialize the units of metrics */
static int 
mf_infiniband_unit_init(metric_units *unit, int infiniband_cid)
{
    /* declare variables */
    int r, retval, code, num_events;
    char event_names[PAPI_MAX_PRESET_EVENTS][PAPI_MAX_STR_LEN];
    char units[PAPI_MAX_PRESET_EVENTS][PAPI_MIN_STR_LEN];
    PAPI_event_info_t evinfo;

    if (unit == NULL) {
        unit = malloc(sizeof(metric_units));
    }
    memset(unit, 0, sizeof(metric_units));

    /* All NATIVE events print units */
    code = PAPI_NATIVE_MASK;
    num_events = 0;

    r = PAPI_enum_cmp_event( &code, PAPI_ENUM_FIRST, infiniband_cid );
    while ( r == PAPI_OK ) {
        retval = PAPI_event_code_to_name( code, event_names[num_events] );
        if ( retval != PAPI_OK ) {
            log_error("ERROR: event_code_to_name failed %s", PAPI_strerror(retval));
            return FAILURE;
        }
        retval = PAPI_get_event_info(code, &evinfo);
        if (retval != PAPI_OK) {
            log_error("ERROR: get_event _info failed %s", PAPI_strerror(retval));
            return FAILURE;
        }
        if(strlen(evinfo.units)==0) {
            r = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, infiniband_cid );
            continue;
        }
        unit->metric_name[num_events] =malloc(64 * sizeof(char));
        strcpy(unit->metric_name[num_events], event_names[num_events]);
        unit->plugin_name[num_events] =malloc(32 * sizeof(char));
        strcpy(unit->plugin_name[num_events], "mf_plugin_infiniband");
        unit->unit[num_events] =malloc(PAPI_MIN_STR_LEN * sizeof(char));
        strncpy(unit->unit[num_events], evinfo.units, sizeof(units[0])-1);

        num_events++;
        r = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, infiniband_cid );
     }

     unit->num_metrics = num_events;
     return SUCCESS;
}

/* Initialize the Infiniband plug-in */
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

/* Samples the registered Infiniband events */
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

/* Conversion of samples data to a JSON document */
char*
mf_infiniband_to_json(INFINIBAND_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(1024 * sizeof(char));
    strcpy(json, "\"type\":\"infiniband\"");

    int idx;
    size_t size = data->num_events;
    for (idx = 0; idx < size; ++idx) {
        sprintf(metric, ",\"%s\":%lld", data->events[idx], data->values[idx]);
        strcat(json, metric);
    }
    free(metric);

    return json;
}

/* Initialize PAPI library */
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

/* Check if Infiniband is initialized */
static int
is_infiniband_initialized()
{
    return is_initialized;
}

/*Stop and clean-up the infiniband plugin */
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
