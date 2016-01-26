/*
 * =======================================================================================
 *
 *      Filename:  threads.h
 *
 *      Description:  Header file of pthread interface module
 *
 *      Version:   3.1.3
 *      Released:  4.11.2014
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Project:  likwid
 *
 *      Copyright (C) 2014 Jan Treibig
 *
 *      This program is free software: you can redistribute it and/or modify it under
 *      the terms of the GNU General Public License as published by the Free Software
 *      Foundation, either version 3 of the License, or (at your option) any later
 *      version.
 *
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY
 *      WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *      PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License along with
 *      this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * =======================================================================================
 */

#ifndef THREADS_H
#define THREADS_H

#include <types.h>
#include <pthread.h>
#include <threads_types.h>
#include <stdio.h>

#define THREADS_BARRIER pthread_barrier_wait(&threads_barrier)

extern pthread_barrier_t threads_barrier;
extern ThreadData* threads_data;
extern ThreadGroup* threads_groups;


/**
 * @brief  Initialization of the thread module
 * @param  numberOfThreads  The total number of threads
 */
extern void threads_init(FILE* OUTSTREAM, int numberOfThreads);

/**
 * @brief  Create all threads
 * @param  startRoutine thread entry function pointer
 */
extern void threads_create(void *(*startRoutine)(void*));

/**
 * @brief  Register User thread data for all threads
 * @param  data  Reference to the user data structo
 * @param  func  Optional function pointer to copy data
 */
extern void threads_registerDataAll(
        ThreadUserData* data,
        threads_copyDataFunc func);

/**
 * @brief  Register User thread data for one thread
 * @param  threadId thread Id 
 * @param  data  Reference to the user data structo
 * @param  func  Optional function pointer to copy data
 */
extern void threads_registerDataThread(
        int threadId,
        ThreadUserData* data,
        threads_copyDataFunc func);

/**
 * @brief  Register User thread data for a thread group
 * @param  groupId  group Id
 * @param  data  Reference to the user data structo
 * @param  func  Optional function pointer to copy data
 */
extern void threads_registerDataGroup(
        int groupId,
        ThreadUserData* data,
        threads_copyDataFunc func);

/**
 * @brief  Join the threads and free pthread related data structures
 * @param
 */
extern void threads_join(void);

/**
 * @brief  Free memory of thread data structures
 * @param  numberOfGroups The number of groups to destroy
 */
extern void threads_destroy(int numberOfGroups);

/**
 * @brief  Create Thread groups
 * @param  numberOfGroups The number of groups to create
 */
extern void threads_createGroups(int numberOfGroups);

#endif /* THREADS_H */
