/*
 * excess_main.h
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#ifndef EXCESS_MAIN_H_
#define EXCESS_MAIN_H_

#include <time.h>

int NUM_THREADS;

long timings[256];

int timingSend, timingCheck;


int readConf(char *confFile);

int getConf(const char *argv[]);

#endif /* EXCESS_MAIN_H_ */
