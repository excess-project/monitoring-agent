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
#include <stdio.h>

//#include "plugin_discover.h"
//#include "plugin_manager.h"

extern int running;

extern char *pwd;

typedef struct metric_t* metric;

struct metric_t {
	struct timespec timestamp;

	char *msg;
};

#endif /* EXCESS_H_ */
