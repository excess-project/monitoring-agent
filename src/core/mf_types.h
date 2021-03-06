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

#ifndef MF_TYPES_H_
#define MF_TYPES_H_

#define METRIC_LENGTH_MAX 128
#define JSON_LENGTH_MAX 2048
/**
 * @brief Pointer to the metric gathered by the plugins
 */
typedef struct metric_t* metric;

/**
 * @brief Structure which represents a value of a metric
 */
struct metric_t {
    struct timespec timestamp;
    char *msg;
} metric_t;

/**
 * @brief Pointer to the metric's unit
 */
typedef struct metric_unit_t metric_units;

/**
 * @brief Structure which represents a unit of a metric
 */
struct metric_unit_t 
{
    char *metric_name[128];
    char *plugin_name[128];
    char *unit[128];
    int num_metrics;
};

#endif