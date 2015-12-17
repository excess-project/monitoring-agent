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
 * number of threads used for monitoring plug-ins
 */
int NUM_THREADS;

/*
 * The array containing the timings of the plug-ins allows to store the timing
 * for up to 254 plug-ins, the first two values are reserved for the timing
 * of the threads which manage the re-read of the configuration file and then
 * sending the gathered data to the server.
 */
extern long timings[256];

/*
 * name of the host the monitoring agent is running on
 */
extern char server_name[256];

/*
 * IP address of the host which will receive the data
 */
extern char addr[100];

/*
 * location of the configuration file
 */
extern char *confFile;

/*
 * a reference to the log file
 */
extern FILE *logFile;

/** @brief Initializing and creating the log file
 *
 * Per default, the log file is created in the folder 'log' within the
 * working directory of the monitoring agent; filenames have the pattern
 * log-<YYYY>-<MM>-<DD>-<HH:MM:SS>
 *
 * @returns 1 on successful file creation; 0 otherwise.
 */
int createLogFile();

/** @brief Perform initialization prior starting monitoring
 *
 * This method performs the following actions: 1) parses the configuration file,
 * 2) reads the host name and current user name, and 3) generates an execution
 * ID if not passed via command line.
 *
 * @returns 1 on success; 0 otherwise
 */
int prepare();

#endif /* _EXCESS_MAIN_H */
