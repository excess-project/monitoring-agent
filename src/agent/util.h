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

/*
 * excess.h
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#ifndef EXCESS_H_
#define EXCESS_H_

#include <time.h>
#include <string.h>
#include <stdio.h>
#include "mf_types.h"

#define START 1
#define STOP 0

/**
 * @brief Flag, which indicates whether monitoring is activated
 */
extern int running;

/**
 * @brief Directory, where the binaries are maintained
 */
extern char *pwd;

/**
 * @brief Gets the fully-qualified domain name
 *
 * @returns a fully-qualified domain name
 */
int getFQDN(char *fqdn);

/**
 * @brief convert timestamp in long double to a string
 */
void convert_time_to_char(long double ts, char* time_stamp);

/**
 * @brief Free the string contained in a metric
 */
void free_metric (metric a_metric);

/**
 * @brief Free a bulk of metrics, with given bulk size
 */
void free_bulk (metric *resMetrics, int size);

#endif /* EXCESS_H_ */
