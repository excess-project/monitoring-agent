/*
 * =======================================================================================
 *
 *      Filename:  perfmon.h
 *
 *      Description:  Header File of perfmon module.
 *                    Configures and reads out performance counters
 *                    on x86 based architectures. Supports multi threading.
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

#ifndef PERFMON_H
#define PERFMON_H

#include <bstrlib.h>
#include <types.h>

extern int perfmon_verbose;

extern void (*perfmon_startCountersThread) (int thread_id);
extern void (*perfmon_stopCountersThread) (int thread_id);
extern int  (*perfmon_getIndex) (bstring reg, PerfmonCounterIndex* index);
extern void (*perfmon_setupCounterThread) (int thread_id, PerfmonEvent* event , PerfmonCounterIndex index);

extern void perfmon_initEventSet(StrUtilEventSet* eventSetConfig, PerfmonEventSet* set);
extern void perfmon_setCSVMode(int v);
extern void perfmon_printAvailableGroups(void);
extern void perfmon_printGroupHelp(bstring group);
extern void perfmon_init(int numThreads, int threads[],FILE* outstream);
extern void perfmon_finalize(void);
extern void perfmon_setupEventSet(bstring eventString, BitMask* mask);
extern double perfmon_getEventResult(int thread, int index);
extern int perfmon_setupEventSetC(char* eventCString, const char*** eventnames);


/*
The following structure and set of functions provide an efficient and easy interface to
access counters from different groups and switch between them.

TODO: The internals need some cleanup, but the interface should remain rather stable.

Usage:
setup = perfmon_prepareEventSetup("VIEW"), etc..
Whenever you want to use one of the prepared setups call:
perfmon_setupCountersForEventSet(setup)

then you can startCounters, stopCounters and then
perfmon_getEventCounterValues() and/or
perfmon_getDerivedCounterValues()
 */
typedef struct {
    const char* groupName;
    int numberOfEvents;
    const char** eventNames;
    int numberOfDerivedCounters;
    const char** derivedNames;    

    // Internal structures DO NOT ACCESS THEM, they need cleanup.
    StrUtilEventSet* eventSetConfig;
    PerfmonEventSet* perfmon_set;
    PerfmonGroup groupSet;
    int groupIndex;
} EventSetup;


extern EventSetup perfmon_prepareEventSetup(char* eventGroupString);
extern void perfmon_setupCountersForEventSet(EventSetup * setup);

// obtain values for all cores, average, min and max for the cores.
extern void perfmon_getEventCounterValues(uint64_t* avg_values, uint64_t* max, uint64_t* min);
extern void perfmon_getDerivedCounterValues(float* avg_values, float* max, float* min);
/////////////////////////

extern void perfmon_setupCounters(void);
extern void perfmon_startCounters(void);
extern void perfmon_stopCounters(void);
extern void perfmon_readCounters(void);
extern double perfmon_getResult(int threadId, char* counterString);
extern void perfmon_printMarkerResults(bstring filepath);
extern void perfmon_logCounterResults(double time);
extern void perfmon_printCounterResults(void);


extern void perfmon_printCounters(void);
extern void perfmon_printEvents(void);

#endif /*PERFMON_H*/
