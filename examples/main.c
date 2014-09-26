/*
 * main.c
 *
 *  Created on: 17.09.2014
 *      Author: hpcneich
 */

#include <stdlib.h>
#include <math.h>

#include "excess_main.h"
#include "util.h"
#include "thread_handler.h"

int main(int argc, const char* argv[]) {

	// example of starting the agent
//	system("/mnt/raid/home/hpcneich/EXCESS/excess/binaries/mf_agent");

	initialise("/mnt/raid/home/hpcneich/EXCESS/excess/binaries/conf");

	startStop("fcnt1", 1);

	// do something in between
	// e.g. fcnt1(void);

	startStop("fcnt1", 0);

	// query database for range of time for all metrics
	char *res = queryRangeFromDB("http://mf.excess-project.eu",
			"JcJNPcVZRa6bVSnn9rejcA", 1411648702.428327942,
			1411648702.429081288);

	//query database for stats of a metric
	char *res1 = querySpecificStatFromDB("http://mf.excess-project.eu",
			"JcJNPcVZRa6bVSnn9rejcA", 1411648702.428327942,
			1411648702.429081288, "mem_used");


	return EXIT_SUCCESS;
}
