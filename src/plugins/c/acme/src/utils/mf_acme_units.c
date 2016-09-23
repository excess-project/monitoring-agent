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

/** @file mf_acme_unit.c
 *   @brief initialize acme units, send to mf_server then
 *
 *   @author Fangli Pi
 */
#include <stdio.h>
#include <stdlib.h> /* malloc, exit, free, ... */

/* monitoring-related includes */
#include "publisher.h"
#include "mf_acme_connector.h" /* mf_acme_is_enabled */

/* A client to initialize acme plugin units of metrics */
int main(void)
{
	printf("[mf_acme_units] starting...\n");
	int ret = unit_file_check("acme");
    if (ret < 0) {
    	printf("unit_file_check failed!\n");
        return -1;
    }
    else if(ret == 0) {
    	printf("unit file does not exist.\n");
    	mf_acme_unit_init();
    }
	return 1;
}
