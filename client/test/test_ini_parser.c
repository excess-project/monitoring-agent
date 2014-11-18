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

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_parse_generic);

    return suite;
}
