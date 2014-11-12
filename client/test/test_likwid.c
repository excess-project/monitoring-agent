#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "CuTest.h"
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
    CuAssertTrue(tc, likwid->hasPP0);
    CuAssertTrue(tc, likwid->hasDRAM);
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
    int duration_in_sec = 1;
    get_power_data(likwid, duration_in_sec);
    printf("%s, %s\n", likwid->sockets[0][0], likwid->sockets[0][1]);
    printf("%s, %s\n", likwid->sockets[0][2], likwid->sockets[0][3]);
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
