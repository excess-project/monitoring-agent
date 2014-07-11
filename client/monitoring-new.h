/*
 * monitoring-new.h
 *
 *  Created on: 11.06.2014
 *      Author: hpcneich
 */

#ifndef MONITORING_NEW_H_
#define MONITORING_NEW_H_

#include "monitoring-excess.h"

#include <fcntl.h>

#define READBUFFER_SIZE 64*1000

typedef unsigned long long jiff;
typedef long double jidd;

double get_cpu_usage(void);

int getPapiValues(const char *papiNames, long_long (*values)[]);

void getprocstat(jiff *restrict cuse, jiff *restrict cice, jiff *restrict csys, jiff *restrict cide, jiff *restrict ciow, jiff *restrict cxxx, jiff *restrict cyyy, jiff *restrict czzz);

void getprocmeminfo(unsigned long *restrict mfre, unsigned long *restrict mtot);

int get_mem_usage(void);

int gatherPapiData(int *EventSet,long_long (*val)[]);

void handle_error(int err);

#endif /* MONITORING_NEW_H_ */
