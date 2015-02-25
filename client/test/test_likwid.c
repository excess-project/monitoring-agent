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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "../libs/CuTest/CuTest.h"
#include "../plugins/likwid_plugin.h"

/* tests for EXCESS testbed at 141.58.0.8 */

void TestLikwid_get_processor_model(CuTest *tc)
{
    char *proc = "Intel Core IvyBridge EP processor";
    CuAssertStrEquals(tc, get_processor_model(), proc);
}

void TestLikwid_is_processor_supported(CuTest *tc)
{
    CuAssertTrue(tc, is_processor_supported());
}

void TestLikwid_check_processor(CuTest *tc)
{
    Likwid_Plugin *likwid = malloc(sizeof(Likwid_Plugin));
    check_processor(likwid);

    CuAssertTrue(tc, likwid->hasDRAM);
    CuAssertTrue(tc, likwid->hasPP0);
    CuAssertTrue(tc, !likwid->hasPP1);

    free(likwid);
}

void TestLikwid_get_power_info(CuTest *tc)
{
    Likwid_Plugin *likwid = malloc(sizeof(Likwid_Plugin));
    get_power_info(likwid);

    // tdp
    CuAssertTrue(tc, likwid->power_values[0] > 0);
    // min power
    float min_power = likwid->power_values[1];
    CuAssertTrue(tc, min_power > 0);
    // max power
    float max_power = likwid->power_values[2];
    CuAssertTrue(tc, max_power > 0 && max_power > min_power);
    // max time window
    CuAssertTrue(tc, likwid->power_values[3] > 0);

    free(likwid);
}

void TestLikwid_get_power_data(CuTest *tc)
{
    Likwid_Plugin *likwid = malloc(sizeof(Likwid_Plugin));
    int duration_in_sec = 2;
    get_power_data(likwid, duration_in_sec);

    int i;
    for (i = 0; i < likwid->numSockets; ++i) {
        // socket
        CuAssertTrue(tc, strtod(likwid->sockets[i][1], NULL) > 0.0);
        CuAssertTrue(tc, likwid->sockets[i][3] > likwid->sockets[i][1]);
        // dram
        if (likwid->hasDRAM) {
            CuAssertTrue(tc, strtod(likwid->dram[i][1], NULL) > 0.0);
            CuAssertTrue(tc, likwid->dram[i][3] > likwid->dram[i][1]);
        }
        // pp0
        if (likwid->hasPP0) {
            CuAssertTrue(tc, strtod(likwid->PP0[i][1], NULL) > 0.0);
            CuAssertTrue(tc, likwid->PP0[i][3] > likwid->PP0[i][1]);
        }
        // pp1
        if (likwid->hasPP1) {
            CuAssertTrue(tc, strtod(likwid->PP1[i][1], NULL) > 0.0);
            CuAssertTrue(tc, likwid->PP1[i][3] > likwid->PP1[i][1]);
        }
    }

    free(likwid);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestLikwid_get_processor_model);
    SUITE_ADD_TEST(suite, TestLikwid_is_processor_supported);
    SUITE_ADD_TEST(suite, TestLikwid_check_processor);
    SUITE_ADD_TEST(suite, TestLikwid_get_power_info);
    SUITE_ADD_TEST(suite, TestLikwid_get_power_data);

    return suite;
}
