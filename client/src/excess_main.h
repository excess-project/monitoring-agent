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
 * excess_main.h
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#ifndef EXCESS_MAIN_H_
#define EXCESS_MAIN_H_

#include <time.h>
#include <stdio.h>

#include <mf_parser.h>

int NUM_THREADS;

/** @brief Array containing the timings of the plugins.
 *
 * 		The array containing the timings of the plugins allows to store the timing
 * 		for up to 254 plugins, the first two values are reserved for the timing
 * 		of the threads which manage the re-read of the config file and the sending
 * 		of the gathered data.
 *
 */
extern long timings[256];

extern char server_name[256];

/** @brief adress of the server which will receive the data
 *
 */
extern char addr[100];

/** @brief location of the config file
 *
 */
extern char *confFile;

/** @brief log file which will contain message by the program
 *
 */
extern FILE *logFile;

/** @brief function printing information about the tool
 *
 * TODO has to be finished
 *
 */
int printHelp();

int createLogFile();

int prepare();

#endif /* EXCESS_MAIN_H_ */
