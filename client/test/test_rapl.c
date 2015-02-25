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


#include <papi.h>
#include <stdlib.h>

#include "../libs/CuTest/CuTest.h"
#include "../plugins/rapl_plugin.h"

void TestRAPL_get_available_events(CuTest *tc)
{
    RAPL_Plugin *rapl = malloc(sizeof(RAPL_Plugin));
    int num_events = get_available_events(rapl);
    CuAssertTrue(tc, num_events == 28); // on EXCESS testbed
    free(rapl);
}

void TestRAPL_get_rapl_component_id(CuTest *tc)
{
    int rapl_id = get_rapl_component_id();
    CuAssertTrue(tc, rapl_id > 0);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestRAPL_get_available_events);
    SUITE_ADD_TEST(suite, TestRAPL_get_rapl_component_id);

    return suite;
}
