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

#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <papi.h>
#include <ctype.h>

#include "../libs/CuTest/CuTest.h"
#include "../plugins/papi_plugin.h"

void TestPAPI_get_named_events(CuTest *tc)
{
    char *events[1] = { "PAPI_TOT_INS"  };
    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    int num_events = sizeof(events) / sizeof(events[0]);
    read_available_named_events(papi, events, num_events);
    CuAssertTrue(tc, papi->num_events == 1);
}

void TestPAPI_get_ALL_PRESETS(CuTest *tc)
{
    char *events[1] = { "ALL_PRESETS" };
    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    int num_events = sizeof(events) / sizeof(events[0]);
    read_available_named_events(papi, events, num_events);
    CuAssertTrue(tc, papi->num_events > 40);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    PAPI_library_init(PAPI_VER_CURRENT);
    SUITE_ADD_TEST(suite, TestPAPI_get_named_events);
    SUITE_ADD_TEST(suite, TestPAPI_get_ALL_PRESETS);

    return suite;
}
