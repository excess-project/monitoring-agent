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
#include "mf_rapl_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

/*
 * declares if the plug-in (i.e., RAPL) is already initialized
 */
static int is_initialized = 0;

/*
 * declares if the RAPL component is enabled to be used for monitoring
 *
 * states: (-1) not initialized, (0) disabled, (1) enabled
 */
static int is_available = -1;

int EventSet = PAPI_NULL;
long long before_time, after_time, elapsed_time;
double denominator;
long long *values;
long long *pre_values;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/
static int mf_rapl_unit_init(metric_units *unit, int rapl_cid);
static int is_rapl_initialized();
static int enable_papi_library();
static double mf_rapl_get_denominator();
static double correct_dram_values(char *event, double value, double pre_value);

/*******************************************************************************
 * mf_rapl_is_enabled
 ******************************************************************************/

int
mf_rapl_is_enabled()
{
    int numcmp, cid;
    const PAPI_component_info_t *cmpinfo = NULL;
    metric_units *RAPL_units = malloc(sizeof(metric_units));

    enable_papi_library();

    if (is_available > -1) {
        return is_available;
    }

    numcmp = PAPI_num_components();
    for (cid = 0; cid < numcmp; cid++) {
        cmpinfo = PAPI_get_component_info(cid);
        if (strstr(cmpinfo->name, "rapl")) {
            if (cmpinfo->disabled) {
                is_available = FAILURE;
                log_warn("Component is DISABLED for this CPU (%d)", cid);
            } else {
                is_available = SUCCESS;
                log_info("Component is ENABLED (%s)", cmpinfo->name);
            }
            /* init rapl all metric names and units */
            mf_rapl_unit_init(RAPL_units, cid);
            /* publish the units to mf_server */
            publish_unit(RAPL_units);
            return is_available;
        }
    }

    is_available = FAILURE;
    return is_available;
}

/*******************************************************************************
 * mf_rapl_unit_init
 ******************************************************************************/
static int 
mf_rapl_unit_init(metric_units *unit, int rapl_cid)
{
    /* declare variables */
    int r, retval, code, num_events;
    char event_names[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
    char units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
    PAPI_event_info_t evinfo;

    if (unit == NULL) {
        unit = malloc(sizeof(metric_units));
    }
    memset(unit, 0, sizeof(metric_units));

    /* All NATIVE events print units */
    code = PAPI_NATIVE_MASK;
    num_events = 0;

    r = PAPI_enum_cmp_event( &code, PAPI_ENUM_FIRST, rapl_cid );
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
            r = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, rapl_cid );
            continue;
        }
        unit->metric_name[num_events] =malloc(64 * sizeof(char));
        strcpy(unit->metric_name[num_events], event_names[num_events]+7);
        unit->plugin_name[num_events] =malloc(32 * sizeof(char));
        strcpy(unit->plugin_name[num_events], "mf_plugin_rapl");
        unit->unit[num_events] =malloc(PAPI_MIN_STR_LEN * sizeof(char));
        if(strstr(unit->metric_name[num_events], "ENERGY:") != NULL) {
            strcpy(unit->unit[num_events], "J");
        }
        else {
            strncpy(unit->unit[num_events], evinfo.units, sizeof(units[0])-1);    
        }
        num_events++;
        r = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, rapl_cid );
     }

     unit->num_metrics = num_events;
     return SUCCESS;
}

/*******************************************************************************
 * mf_rapl_init
 ******************************************************************************/

int
mf_rapl_init(RAPL_Plugin *data, char **rapl_events, size_t num_events)
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

    /*
     * set denominator for DRAM values based on the current CPU model
     */
    denominator = mf_rapl_get_denominator();
    values = calloc(registered_idx, sizeof(long long));
    pre_values = calloc(registered_idx, sizeof(long long));
    /*initialize pre_values to 0*/
    for (idx = 0; idx != num_events; ++idx) {
        pre_values[idx]=0;
    }

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
 * mf_rapl_sample
 ******************************************************************************/

int
mf_rapl_sample(RAPL_Plugin *data)
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
    int retval = PAPI_accum(EventSet, values);
    if (retval != PAPI_OK) {
        return FAILURE;
    }

    /*
     * account for time passed between last measurement and now
     */
    int idx;
    elapsed_time = ((double) (after_time - before_time)); //in nano second
    for (idx = 0; idx < size; ++idx) {
        data->values[idx] = correct_dram_values(data->events[idx], values[idx], pre_values[idx]);
        pre_values[idx] = values[idx];
        values[idx] = 0;
    }

    /*
     * update time interval
     */
    before_time = after_time;

    return SUCCESS;
}

/*******************************************************************************
 * mf_rapl_to_json
 ******************************************************************************/

char*
mf_rapl_to_json(RAPL_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, "\"type\":\"energy\"");

    int idx;
    size_t size = data->num_events;
    for (idx = 0; idx < size; ++idx) {
        sprintf(metric, ",\"%s\":%.4f", data->events[idx], data->values[idx]);
        strcat(json, metric);
        //if metric is energy, send also power value
        char *p = strstr(data->events[idx], "ENERGY");
        if (p != NULL) {
            double power_value = (double) data->values[idx] * 1.0e9 / elapsed_time;
            //nano joule / nano second = watt
            char event[40] = {'\0'};
            strncpy(event, data->events[idx], (p - data->events[idx]));
            strcat(event, "POWER");
            strcat(event, p+6);
            sprintf(metric, ",\"%s\":%.4f", event, power_value);
            strcat(json, metric);
        }
    }
    free(metric);

    return json;
}

/*******************************************************************************
 * correct_dram_values
 ******************************************************************************/

static double
correct_dram_values(char *event, double value, double pre_value)
{
    double ret;
    if (strcmp(event, "DRAM_ENERGY:PACKAGE0") == 0 ||
        strcmp(event, "DRAM_ENERGY:PACKAGE1") == 0) {
        ret = (double) (value / denominator);
    }
    if(strstr(event, "ENERGY") != NULL) {
        ret = (double) (value - pre_value) * 1.0e-9; //change from nJ to J
    }
    return ret;
}

/*******************************************************************************
 * enable_papi_library
 ******************************************************************************/

static int
enable_papi_library()
{
    if (is_rapl_initialized()) {
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
 * is_rapl_initialized
 ******************************************************************************/

static int
is_rapl_initialized()
{
    return is_initialized;
}

/*******************************************************************************
 * mf_rapl_shutdown
 ******************************************************************************/

void
mf_rapl_shutdown()
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

/*******************************************************************************
 * native_cpuid
 ******************************************************************************/

void
native_cpuid(
    unsigned int *eax,
    unsigned int *ebx,
    unsigned int *ecx,
    unsigned int *edx)
{
    asm volatile("cpuid"
        : "=a" (*eax),
          "=b" (*ebx),
          "=c" (*ecx),
          "=d" (*edx)
        : "0" (*eax), "2" (*ecx)
    );
}

/*******************************************************************************
 * mf_rapl_get_cpu_model
 ******************************************************************************/

static int
mf_rapl_get_cpu_model()
{
    unsigned eax, ebx, ecx, edx;
    eax = 1; /* set processor info and feature bits */
    native_cpuid(&eax, &ebx, &ecx, &edx);
    return (eax >> 4) & 0xF;
}

/*******************************************************************************
 * mf_rapl_get_denominator
 ******************************************************************************/

static double
mf_rapl_get_denominator()
{
    int cpu_model = mf_rapl_get_cpu_model();
    if (cpu_model == 15) {
        return 15.3;
    } else {
        return 1.0;
    }
}
