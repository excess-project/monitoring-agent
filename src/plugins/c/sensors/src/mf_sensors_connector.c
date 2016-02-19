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

#include <sensors.h>
#include <error.h>

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_sensors_connector.h"

#define SUCCESS 1
#define FAILURE 0

// https://searchcode.com/codesearch/view/68001947/#l-290
// http://dev.man-online.org/man3/libsensors/

/*******************************************************************************
 * Variable Declarations
 ******************************************************************************/

typedef struct requested_features_t {
    const sensors_chip_name *chip;
    const sensors_subfeature *subfeature;
    char *label;
    struct requested_features_t *next;
} requested_features;

requested_features *features = NULL;

/*******************************************************************************
 * Forward Declarations
 ******************************************************************************/

int
prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre));
}

requested_features*
create_feature(const sensors_chip_name* chip, const sensors_subfeature *subfeature, char *label) {
    requested_features* list = malloc(sizeof(requested_features));
    if (list != NULL) {
        list->chip = chip;
        list->subfeature = subfeature;
        list->label = label;
        list->next = NULL;
    }

    return list;
}

requested_features*
add_feature(requested_features* list, const sensors_chip_name* chip, const sensors_subfeature *subfeature, char *label) {
    requested_features* newList = create_feature(chip, subfeature, label);
    if (newList != NULL) {
        newList->next = list;
    }

    return newList;
}

/*******************************************************************************
 * mf_sensors_is_enabled
 ******************************************************************************/

int
mf_sensors_is_enabled()
{
    return 1;
}

/*******************************************************************************
 * mf_sensors_init
 ******************************************************************************/

int
mf_sensors_init(SENSORS_Plugin *data, char **sensors_events, size_t num_events)
{
    int retval = sensors_init(NULL);
    if (retval != 0) {
        log_error("ERROR: Couldn't initiate lm-sensors: %s",
            sensors_strerror(retval));
        return FAILURE;
    }

    const sensors_chip_name *chip;
    int chip_num = 0;
    while ((chip = sensors_get_detected_chips(NULL, &chip_num)) != NULL) {
        if (strcmp(chip->prefix, "coretemp") != 0) {
            continue;
        }

        const sensors_feature *feature;
        int feature_num = 0;

        while ((feature = sensors_get_features(chip, &feature_num)) != NULL) {
            const sensors_subfeature *subfeature;
            int subfeature_num = 0;

            if (feature->type != SENSORS_FEATURE_TEMP) {
                continue;
            }

            while ((subfeature = sensors_get_all_subfeatures (chip, feature, &subfeature_num)) != NULL) {
                if (subfeature->type != SENSORS_SUBFEATURE_TEMP_INPUT) {
                    continue;
                }

                char* label = sensors_get_label(chip, feature);

                int i = 0;
                int flag = 0;
                for (i=0; i<num_events; i++) {
                    if(strcmp(sensors_events[i], label) == 0) {
                        flag=1;
                        break;
                    }
                }
                if(flag == 0) {
                    continue;
                }

                //if (prefix("Core", label) != 0) {
                //    continue;
                //}

                if (features == NULL) {
                    features = create_feature(chip, subfeature, label);
                } else {
                    features = add_feature(features, chip, subfeature, label);
                }

                break;
            }
        }
    }

    return SUCCESS;
}

/*******************************************************************************
 * mf_sensors_sample
 ******************************************************************************/

int
mf_sensors_sample(SENSORS_Plugin *data)
{
    double value;
    int i;
    requested_features *iter;
    for (i = 0, iter = features; iter != NULL; iter = iter->next, i++) {
        sensors_get_value(iter->chip, iter->subfeature->number, &value);
        data->events[i] = malloc(16);
        strcpy(data->events[i], iter->label);
        data->values[i]=value;
        printf("%s %s %.2f\n", iter->label, iter->subfeature->name, value);
    }
    data->num_events = i+1;

    return 1;
}

/*******************************************************************************
 * mf_sensors_to_json
 ******************************************************************************/

char*
mf_sensors_to_json(SENSORS_Plugin *data)
{
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"temperature\"");
    int i;
    for (i = 0; i < data->num_events && data->events[i] != NULL; i++) {
        sprintf(metric, ",\"%s\":%.2f", data->events[i], data->values[i]);
        strcat(json, metric);
    }

    free(metric);

    return json;
}

/*******************************************************************************
 * mf_sensors_shutdown
 ******************************************************************************/

void
mf_sensors_shutdown()
{
    sensors_cleanup();
}