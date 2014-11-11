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
    int i;
    for (i = 0; i < 4; ++i) {
        printf("name: %s, value: %g\n", likwid->power_names[i], likwid->power_values[i]);
    }
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestLikwid_get_processor_model);
    SUITE_ADD_TEST(suite, TestLikwid_is_processor_supported);
    SUITE_ADD_TEST(suite, TestLikwid_check_processor);
    SUITE_ADD_TEST(suite, TestLikwid_get_power_info);

    return suite;
}
