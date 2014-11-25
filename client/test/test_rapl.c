
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
