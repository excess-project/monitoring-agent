#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <papi.h>
#include <ctype.h>

#include "CuTest.h"
#include "../plugins/papi_plugin.h"
#include "../plugins/parser.h"


void TestPAPI_get_named_events(CuTest *tc)
{
    char *events[2] = { "PAPI_TOT_INS", "PAPI_SP_OPS" };
    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    int num_events = sizeof(events) / sizeof(events[0]);
    read_available_named_events(papi, events, num_events);
    CuAssertTrue(tc, papi->num_events == 2);
}

void TestPAPI_get_ALL_PRESETS(CuTest *tc)
{
    char *events[1] = { "ALL_PRESETS" };
    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    int num_events = sizeof(events) / sizeof(events[0]);
    read_available_named_events(papi, events, num_events);
    CuAssertTrue(tc, papi->num_events == 52);
}

void TestPAPI_fetch_events_defined_via_conf(CuTest *tc)
{
    Parser *parser = get_instance();
    read_PAPI_events_from_file(parser, "pluginConf");
    // Events: PAPI_TOT_INS, PAPI_SP_OPS, PAPI_TOT_CYC

    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    read_available_named_events(papi, parser->events, parser->metrics_count);
    CuAssertTrue(tc, papi->num_events == 2); // PAPI_TOT_CYC is not supported
}

void TestPAPI_fetch_preset_events_defined_via_conf(CuTest *tc)
{
    Parser *parser = get_instance();
    read_PAPI_events_from_file(parser, "pluginConf_all");
    // Events: ALL_PRESETS

    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    read_available_named_events(papi, parser->events, parser->metrics_count);
    CuAssertTrue(tc, papi->num_events == 52);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestPAPI_get_named_events);
    SUITE_ADD_TEST(suite, TestPAPI_get_ALL_PRESETS);
    SUITE_ADD_TEST(suite, TestPAPI_fetch_events_defined_via_conf);
    SUITE_ADD_TEST(suite, TestPAPI_fetch_preset_events_defined_via_conf);

    return suite;
}
