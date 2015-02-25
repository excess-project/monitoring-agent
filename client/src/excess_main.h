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

#ifndef EXCESS_MAIN_H_
#define EXCESS_MAIN_H_

#include <time.h>
#include <stdio.h>

#include <mf_parser.h>

int NUM_THREADS;

/**
 * @brief Array containing the timings of the plug-ins.
 *
 * The array containing the timings of the plug-ins allows to store the timing
 * for up to 254 plug-ins, the first two values are reserved for the timing
 * of the threads which manage the re-read of the configuration file and then
 * sending the gathered data to the server.
 *
 */
extern long timings[256];

/**
 * @brief The server name
 */
extern char server_name[256];

/**
 * @brief Address of the server which will receive the data
 */
extern char addr[100];

/**
 * @brief Location of the configuration file
 */
extern char *confFile;

/**
 * @brief Log file
 */
extern FILE *logFile;

/**
 * @brief Initializes and creates the log files
 *
 * @returns 1 on success; 0 otherwise.
 */
int createLogFile();

/**
 * @brief Initializes all variables prior to starting the MFAgent
 *
 * This function parses the configuration file, sets the host name, user name,
 * and the server name for further communication.
 *
 * @returns 1 on success; 0 otherwise.
 */
int prepare();

#endif /* EXCESS_MAIN_H_ */
