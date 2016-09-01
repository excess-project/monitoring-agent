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

/** @file mf_sensors_unit.c
 *   @brief initialize sensors units, send to mf_server then
 *
 *   @author Fangli Pi
 */
#include <stdio.h>
#include <stdlib.h> /* malloc, exit, free, ... */

/* monitoring-related includes */
#include "mf_sensors_connector.h" /* mf_sensors_is_enabled */

/* A client to initialize sensors plugin units of metrics */
int main(void)
{
	printf("[mf_sensors_units] starting...\n");
	int ret = mf_sensors_is_enabled();
	if(ret != 1) {
		printf("[mf_sensors_units] units init failed.\n");
		return -1;
	}
	return 1;
}
