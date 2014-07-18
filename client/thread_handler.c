/*
 * thread_handler.c
 *
 *  Created on: 17.07.2014
 *      Author: hpcneich
 */

#include "thread_handler.h"
#include "util.h"

int running;

int startThreads() {
	running = 1;




	return 1;
}
int checkConf() {
	while (running)
		;
	return 1;
}
