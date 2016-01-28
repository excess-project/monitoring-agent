/*
 * =======================================================================================
 *
 *      Filename:  affinity_types.h
 *
 *      Description:  Type Definitions for affinity Module
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

#ifndef AFFINITY_TYPES_H
#define AFFINITY_TYPES_H

typedef struct {
    bstring tag;
    uint32_t numberOfProcessors;
    uint32_t numberOfCores;
    int* processorList;
} AffinityDomain;


#endif /*AFFINITY_TYPES_H*/
