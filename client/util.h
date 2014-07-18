/*
 * excess.h
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#ifndef EXCESS_H_
#define EXCESS_H_

#include <time.h>
#include <string.h>

extern int running;

struct metric_t *metricPtr;
typedef struct metric {
	struct timespec timestamp;

	char *msg;
} metric_t;

#endif /* EXCESS_H_ */
