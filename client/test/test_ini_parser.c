#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"
#include "../ini_parser.h"


void Test_parse_generic(CuTest *tc)
{
    generic config;
    parse_generic("basic.ini", &config);
    CuAssertStrEquals(tc, "http://141.58.0.2:3000/executions/", config.hostname);
    CuAssertStrEquals(tc, "0s", config.update_interval);
    CuAssertStrEquals(tc, "30s", config.update_config);
}

void Test_parse_timings(CuTest *tc)
{
    timings config;
    parse_timings("basic.ini", &config);
    CuAssertStrEquals(tc, "1000000000ns", config.default_timing);
    CuAssertStrEquals(tc, "5000000000ns", config.papi);
    CuAssertStrEquals(tc, "1000000000ns", config.rapl);
    CuAssertStrEquals(tc, "1000000000ns", config.likwid);
    CuAssertStrEquals(tc, "1000000000ns", config.mem_info);
    CuAssertStrEquals(tc, "1000000000ns", config.hw_power);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_parse_generic);
    SUITE_ADD_TEST(suite, Test_parse_timings);

    return suite;
}
