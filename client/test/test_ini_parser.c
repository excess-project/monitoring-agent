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

void Test_parse_plugins(CuTest *tc)
{
    plugins config;
    parse_plugins("basic.ini", &config);
    CuAssertStrEquals(tc, "off", config.papi);
    CuAssertStrEquals(tc, "off", config.rapl);
    CuAssertStrEquals(tc, "off", config.likwid);
    CuAssertStrEquals(tc, "on" , config.mem_info);
    CuAssertStrEquals(tc, "off", config.hw_power);
}

void Test_parse_papi(CuTest *tc)
{
    int i;
    plugin config;
    parse_plugin("basic.ini", "papi", &config);
    for (i = 0; i < config.size; ++i) {
        if (strcmp(config.events[i],  "PAPI_DP_OPS") == 0) {
            CuAssertStrEquals(tc, "on", config.values[i]);
        }
    }
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_parse_generic);
    SUITE_ADD_TEST(suite, Test_parse_timings);
    SUITE_ADD_TEST(suite, Test_parse_plugins);
    SUITE_ADD_TEST(suite, Test_parse_papi);

    return suite;
}
