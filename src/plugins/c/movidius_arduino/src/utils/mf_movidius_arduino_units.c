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

/** @file mf_movidius_arduino_unit.c
 *   @brief initialize movidius units, send to mf_server then
 *
 *   @author Fangli Pi
 */

#include <stdio.h>
#include <stdlib.h> /* malloc, exit, free, ... */

/* monitoring-related includes */
#include "mf_movidius_arduino_connector.h" /* mf_rapl_is_enabled */

/*******************************************************************************
 * Main
 ******************************************************************************/

int main(void)
{
	printf("[mf_movidius_arduino_units] starting...\n");
	int ret = mf_movi_unit_init();
	if(ret != 1) {
		printf("[mf_movidius_arduino_units] units init failed.\n");
		return -1;
	}
	return 1;
}
