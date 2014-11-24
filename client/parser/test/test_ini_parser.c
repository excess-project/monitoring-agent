#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../libs/CuTest/CuTest.h"
#include "../src/ini_parser.h"


void Test_parse_generic(CuTest *tc)
{
    config_generic config;
    int retval = parse_generic("config.ini", &config);
    CuAssertTrue(tc, retval);
    CuAssertStrEquals(tc, "http://141.58.0.2:3000/executions/", config.server);
}

void Test_parse_timings(CuTest *tc)
{
    config_timings config;
    int retval = parse_timings("config.ini", &config);
    CuAssertTrue(tc, retval);
    CuAssertTrue(tc,  0 == config.update_interval);
    CuAssertTrue(tc, 30 == config.update_config);
    CuAssertTrue(tc, 1000000000 == config.default_timing);
    CuAssertTrue(tc,  200000000 == config.papi);
    CuAssertTrue(tc,  400000000 == config.rapl);
    CuAssertTrue(tc, 1000000000 == config.likwid);
    CuAssertTrue(tc,  300000000 == config.mem_info);
    //CuAssertTrue(tc, 1000000000 == config.hw_power);
}

void Test_parse_plugins(CuTest *tc)
{
    config_plugins config;
    int retval = parse_plugins("config.ini", &config);
    CuAssertTrue(tc, retval);
    CuAssertStrEquals(tc, "off", config.papi);
    CuAssertStrEquals(tc, "off", config.rapl);
    CuAssertStrEquals(tc, "off", config.likwid);
    CuAssertStrEquals(tc, "on" , config.mem_info);
    CuAssertStrEquals(tc, "off", config.hw_power);
}

void Test_parse_papi(CuTest *tc)
{
    config_plugin config;
    int retval = parse_plugin("config.ini", "papi", &config);
    CuAssertTrue(tc, retval);    
    CuAssertTrue(tc, 1 == config.size);
    CuAssertStrEquals(tc, "PAPI_DP_OPS", config.events[0]);
}

void Test_parse_plugin_without_section(CuTest *tc)
{
    config_plugin config;
    int retval = parse_plugin("config.ini", "hw_power", &config);
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
