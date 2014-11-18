#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"
#include "../ini_parser.h"


void Test_parse_generic(CuTest *tc)
{
    generic config;
    int retval = parse_generic("basic.ini", &config);
    CuAssertTrue(tc, retval);
    CuAssertStrEquals(tc, "http://141.58.0.2:3000/executions/", config.hostname);
    CuAssertStrEquals(tc, "0s", config.update_interval);
    CuAssertStrEquals(tc, "30s", config.update_config);
}

void Test_parse_timings(CuTest *tc)
{
    timings config;
    int retval = parse_timings("basic.ini", &config);
    CuAssertTrue(tc, retval);
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
    int retval = parse_plugins("basic.ini", &config);
    CuAssertTrue(tc, retval);
    CuAssertStrEquals(tc, "off", config.papi);
    CuAssertStrEquals(tc, "off", config.rapl);
    CuAssertStrEquals(tc, "off", config.likwid);
    CuAssertStrEquals(tc, "on" , config.mem_info);
    CuAssertStrEquals(tc, "off", config.hw_power);
}

void Test_parse_papi(CuTest *tc)
{
    plugin config;
    int retval = parse_plugin("basic.ini", "papi", &config);
    CuAssertTrue(tc, retval);    
    CuAssertTrue(tc, 1 == config.size);
    CuAssertStrEquals(tc, "PAPI_DP_OPS", config.events[0]);
}

void Test_parse_plugin_without_section(CuTest *tc)
{
    plugin config;
    int retval = parse_plugin("basic.ini", "hw_power", &config);
    CuAssertTrue(tc, !retval);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_parse_generic);
    SUITE_ADD_TEST(suite, Test_parse_timings);
    SUITE_ADD_TEST(suite, Test_parse_plugins);
    SUITE_ADD_TEST(suite, Test_parse_papi);
    SUITE_ADD_TEST(suite, Test_parse_plugin_without_section);

    return suite;
}
