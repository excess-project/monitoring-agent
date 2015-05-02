/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MF_IOSTAT_CONNECTOR_H_
#define MF_IOSTAT_CONNECTOR_H_

typedef struct Iostat_Plugin_t Iostat_Plugin;

struct Iostat_Plugin_t
{
    char* events[32];
    char* values[32];
    int num_events;
};

void mf_iostat_init(char** named_events, size_t num_events);

/**
 * @brief Reads the given named events by calling the IOSTAT program with the
 *        given time interval.
 *
 */
void mf_iostat_read(Iostat_Plugin *iostat);


#endif
