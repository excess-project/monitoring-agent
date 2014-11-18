#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"
#include "../ini_parser.h"


void Test_parse(CuTest *tc)
{
    configuration config;
    CuAssertTrue(tc, parse("config.ini", &config));
}

void Test_parse_file_not_exists(CuTest *tc)
{
    configuration config;
    CuAssertTrue(tc, !parse("some.ini", &config));
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_parse);
    SUITE_ADD_TEST(suite, Test_parse_file_not_exists);

    return suite;
}
