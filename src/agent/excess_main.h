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

/** @file excess_main.h
 *  @brief Provides a basic interface to start monitoring.
 *
 *  This interface declares essential methods and functions used to enable
 *  monitoring. Moreover, shared variables such as server_name are declare,
 *  that will be used by other classes as well.
 *
 *  @author Nico Eichhorn (eichhorn)
 *  @author Anthony Sulistio (sulisto)
 *
 *  @bug No known bugs.
 */

#ifndef _EXCESS_MAIN_H
#define _EXCESS_MAIN_H

#include <stdio.h>
#include <time.h>

/* monitoring related includes */
#include <mf_parser.h>

/*
 * The array containing the timings of the plug-ins allows to store the timing
 * for up to 254 plug-ins, the first two values are reserved for the timing
 * of the threads which manage the re-read of the configuration file and then
 * sending the gathered data to the server.
 */
extern long timings[256];

/*
 * The timing for publish_json is measured
 */
extern double publish_json_time;

/*
 * name of the host the monitoring agent is running on
 */
extern char server_name[256];

/*
 * location of the configuration file
 */
extern char *confFile;

/*
 * a reference to the log file
 */
extern FILE *logFile;

/*
 * a reference to current workflow/user id
 */
extern char* workflow;

/*
 * a reference to current experiment id
 */
extern char* experiment_id;
/*
 * a reference to the current task/application id
 */
extern char* task;

/*
 * a reference to the name of the host
 */
extern char* hostname;

/*
 * a reference to the mf api version
 */
extern char* api_version;


#endif /* _EXCESS_MAIN_H */
