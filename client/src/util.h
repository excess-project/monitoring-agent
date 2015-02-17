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
 * @brief get fully-qualified domain name
 */
int getFQDN(char *fqdn);

#endif /* EXCESS_H_ */
