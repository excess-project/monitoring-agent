/** ***************************************************************************
 * \file
 * \brief monitoring_EXCESS API header
 */

#ifndef _MONITORING_EXCESS_H
#define _MONITORING_EXCESS_H

/** standards includes */

#include <stdio.h>
#include <stdlib.h>				
#include <string.h>				
#include <stdint.h>				

#include <errno.h>				
#include <unistd.h>				
#include <signal.h>				
#include <semaphore.h>			
#include <pthread.h>			
#include <sys/time.h>			
#include <math.h>

#define TIME_OUT  1000000   //microseconde  
#define INPUT_SIZE 200			
#define PATH_MAX_SIZE 200
#define BUFFER_SIZE 1000
#define DATA_AVAILABLE 0 

int BEGIN_INDEX;
int CURRENT_INDEX;
int END_INDEX;

int DATA_EMPTY;

typedef struct {
	double cpu_user;
	double cpu_sys;
	double cpu_nice;
	double cpu_idle;
	double cpu_wa_io;

	int diff_cpu;
	struct timeval cpu_time;

} cpu_i;

typedef struct {
	uint32_t memory_free;
	uint32_t memory_used;
	uint32_t memory_total;
	uint32_t memory_buffers;

	uint32_t swap_free;
	uint32_t swap_used;
	uint32_t swap_total;
	uint32_t swap_cached;

	int diff_mem;
	struct timeval mem_time;

} mem_i;

/************************  the information collected by the monitoring sensor  *************************/

/*** sensor structure message - collected information and related state  ****/

typedef struct sensor_msg {

	/** memory state  */
	uint32_t ram_used;
	uint32_t ram_avail;
	struct timeval mem_time;

	/** cpu state  */
	double cpu_avail;
	double cpu_used;
	double cpu_wa_io;
	struct timeval cpu_time;

	int time_to_get_cpu_info;
	int time_to_get_mem_info;

} sensor_msg_t;

/*** the reserved information between sensor and client part  ****/

sensor_msg_t to_send_msg[BUFFER_SIZE];

/*****************************************************************
 * defined functions to get CPU and memory collected information 
 *****************************************************************/

int32_t get_mem_info(mem_i *infos_mem);

int32_t get_cpu_info(cpu_i *infos_cpu);

/*****************************************************************
 *  excess_sensor : is the monitoring information gatherer 
 *****************************************************************/

int excess_sensor(sensor_msg_t * info);

/*****************************************************************
 *  excess_client_part : used to communicate with the server part (distant) 
 *****************************************************************/

int excess_client_part(void * info);

#endif /* _MONITORING_EXCESS_H */

