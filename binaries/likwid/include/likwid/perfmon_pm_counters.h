/*
 * =======================================================================================
 *
 *      Filename:  perfmon_pm_counters.h
 *
 *      Description: Counter Header File of perfmon module.
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

#define NUM_COUNTERS_PM 2
#define NUM_COUNTERS_CORE_PM 2

static PerfmonCounterMap pm_counter_map[NUM_COUNTERS_PM] = {
    {"PMC0",PMC0, PMC, MSR_PERFEVTSEL0, MSR_PMC0, 0, 0},
    {"PMC1",PMC1, PMC, MSR_PERFEVTSEL1, MSR_PMC1, 0, 0}
};

