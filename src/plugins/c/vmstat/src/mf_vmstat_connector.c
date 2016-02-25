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
#include <unistd.h> /* access */

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_vmstat_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

static const char *VMSTAT_BINARY = "/usr/bin/vmstat";
static const char *PROC_VMSTAT = "/proc/vmstat";

/*******************************************************************************
 * mf_vmstat_is_enabled
 ******************************************************************************/

int
mf_vmstat_is_enabled()
{
    if (access(VMSTAT_BINARY, F_OK|X_OK) == 0) {
        log_info(
            "VMSTAT >> utility is installed and executable '%s'",
            VMSTAT_BINARY
        );
        return SUCCESS;
    } else {
        log_error("VMSTAT >> ERROR while trying to execute %s", VMSTAT_BINARY);
        return FAILURE;
    }
}

/*******************************************************************************
 * mf_vmstat_init
 ******************************************************************************/

int
mf_vmstat_init(VMSTAT_Plugin *data, char **vmstat_events, size_t num_events)
{
    int i;
    char vmstat_metric[128];

    /*
     * check if data is initialized
     */
    if (data == NULL) {
        data = malloc(sizeof(VMSTAT_Plugin));
    }
    memset(data, 0, sizeof(VMSTAT_Plugin));
    data->num_events = 0;

    /*
     * validate given user-defined events
     */
    FILE *fp = fopen(PROC_VMSTAT, "r");
    while (fscanf(fp, "%s", vmstat_metric) == 1) {
        for (i = 0; i != num_events; ++i) {
            if (strcmp(vmstat_metric, vmstat_events[i]) == 0) {
                log_info("VMSTAT >> Adding event %s", vmstat_metric);

                data->events[data->num_events] = malloc(sizeof(char) * 256);
                strcpy(data->events[data->num_events], vmstat_metric);
                data->num_events++;

                /*
                 * remove event if it could be registered
                 */
                vmstat_events[i][0] = '\0';
                break;
            }
        }
    }
    fclose(fp);

    /*
     * print out which events could not be registered
     */
    int retval = SUCCESS;
    for (i = 0; i != num_events; ++i) {
        if (vmstat_events[i][0] != '\0') {
            log_warn(
                "VMSTAT >> Could not add event '%s'. Not found.",
                vmstat_events[i]
            );
            retval = FAILURE;
        }
    }

    return retval;
}

/*******************************************************************************
 * mf_vmstat_sample
 ******************************************************************************/

int
mf_vmstat_sample(VMSTAT_Plugin *data)
{
    int i;
    char vmstat_metric[128];
    long long value = 0;

    FILE *fp = fopen(PROC_VMSTAT, "r");
    while (fscanf(fp, "%s %lld", vmstat_metric, &value) == 2) {
        for (i = 0; i != data->num_events; ++i) {
            if (strcmp(vmstat_metric, data->events[i]) == 0) {
                data->values[i] = value;
                break;
            }
        }
    }
    fclose(fp);

    return SUCCESS;
}

/*******************************************************************************
 * mf_vmstat_to_json
 ******************************************************************************/

const char*
mf_vmstat_to_json(VMSTAT_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, "\"type\":\"io\"");

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
 * mf_vmstat_shutdown
 ******************************************************************************/

void
mf_vmstat_shutdown()
{
    /*
     * nothing to do
     */
}