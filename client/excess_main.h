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

#include <ini_parser.h>

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

extern config_generic conf_generic;
extern config_timings conf_timings;


/** @brief function printing information about the tool
 *
 * TODO has to be finished
 *
 */
int printHelp();


#endif /* EXCESS_MAIN_H_ */
