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

#define START 1
#define STOP 0

/**
 * @brief flags which indicates whether gathering cycle is running or not
 */
extern int running;

/**
 * @brief directory where the binary is located
 */
extern char *pwd;

/**
 * @brief pointer to the metric gathered by the plugins
 */
typedef struct metric_t* metric;

/**
 * @brief structure which represents a value of a metric
 */
struct metric_t {
	struct timespec timestamp;

	char *msg;
} metric_t;

/**
 * query database to get a range of datasets
 */
extern char* queryRangeFromDB(const char *host, const char *id, long double t0,
		long double t1);

/**
 * query database to get a special range of one dataset
 *
 * not working on the serverside yet
 */
extern char* querySpecificStatFromDB(const char *URL,const char *id, long double t0,
		long double t1, char *metric);

/**
 * @brief function which creates values when a function has started/stopped
 */
int startStop(const char *fnctName, int flag);

#endif /* EXCESS_H_ */
