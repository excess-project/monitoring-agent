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
#include "mf_meminfo_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * VARIABLE DECLARATIONS
 ******************************************************************************/

static const char *PROC_MEMINFO = "/proc/meminfo";

/*******************************************************************************
 * mf_meminfo_sample
 ******************************************************************************/

int
mf_meminfo_is_enabled()
{
    FILE *fp = fopen(PROC_MEMINFO, "r");
    if (!fp) {
        log_error("MEM_INFO >> Error while trying to execute %s", PROC_MEMINFO);
        return FAILURE;
    }
    fclose(fp);

    return SUCCESS;
}

/*******************************************************************************
 * mf_meminfo_init
 ******************************************************************************/

int
mf_meminfo_init(MEMINFO_Plugin *data, char **meminfo_events, size_t num_events)
{
    int i;
    char meminfo_metric[128];

    /*
     * check if data is initialized
     */
    if (data == NULL) {
        data = malloc(sizeof(MEMINFO_Plugin));
    }
    memset(data, 0, sizeof(MEMINFO_Plugin));
    data->num_events = 0;

    /*
     * validate given user-defined events
     */
    FILE *fp = fopen(PROC_MEMINFO, "r");
    while (fscanf(fp, "%s", meminfo_metric) == 1) {
        /*
         * remove ":" from metric name
         */
        if (strlen(meminfo_metric) >= 2) {
            meminfo_metric[strlen(meminfo_metric)-1] = 0;
        }

        for (i = 0; i != num_events; ++i) {
            if (strcmp(meminfo_metric, meminfo_events[i]) == 0) {
                log_info("MEMINFO >> Adding event %s", meminfo_metric);

                data->events[data->num_events] = malloc(sizeof(char) * 256);

                strcpy(data->events[data->num_events], meminfo_metric);
                data->num_events++;

                /*
                 * remove event if it could be registered
                 */
                meminfo_events[i][0] = '\0';
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
        if (meminfo_events[i][0] != '\0') {
            log_warn(
                "MEMINFO >> Could not add event '%s'. Not found.",
                meminfo_events[i]
            );
            retval = FAILURE;
        }
    }

    return retval;
}


/*******************************************************************************
 * mf_meminfo_sample
 ******************************************************************************/

int
mf_meminfo_sample(MEMINFO_Plugin *data)
{
    int i;
    char meminfo_metric[128];
    char dimension[32];
    long long value = 0;

    FILE *fp = fopen(PROC_MEMINFO, "r");
    while (fscanf(fp, "%s %lld %s", meminfo_metric, &value, dimension) == 3) {
        /*
         * remove ":" from metric name
         */
        if (strlen(meminfo_metric) >= 2) {
            meminfo_metric[strlen(meminfo_metric)-1] = 0;
        }

        for (i = 0; i != data->num_events; ++i) {
            if (strcmp(meminfo_metric, data->events[i]) == 0) {
                data->values[i] = value;
                break;
            }
        }
    }
    fclose(fp);

    return SUCCESS;
}

/*******************************************************************************
 * mf_meminfo_to_json
 ******************************************************************************/

const char*
mf_meminfo_to_json(MEMINFO_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"memory\"");

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
 * mf_meminfo_shutdown
 ******************************************************************************/

void
mf_meminfo_shutdown()
{
    /*
     * nothing to do
     */
}