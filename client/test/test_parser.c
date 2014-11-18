#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../plugins/parser.h"
#include "CuTest.h"


void TestParser_read_PAPI_one_event_from_line(CuTest *tc)
{
	char *line = "PAPI_TOT_INS";
    Parser *parser = get_instance();
    read_PAPI_events_from_line(parser, line);
    CuAssertTrue(tc, parser->metrics_count == 1);
}

void TestParser_read_PAPI_two_events_from_line(CuTest *tc)
{
	char *line = "PAPI_TOT_INS PAPI_TOT_CYC";
    Parser *parser = get_instance();
    read_PAPI_events_from_line(parser, line);
    CuAssertTrue(tc, parser->metrics_count == 2);
}

void TestParser_read_PAPI_events_from_line_with_spaces(CuTest *tc)
{
	char *line = " PAPI_TOT_INS ";
	Parser *parser = get_instance();
	read_PAPI_events_from_line(parser, line);
	CuAssertTrue(tc, parser->metrics_count == 1);
}

void TestParser_read_PAPI_events_from_file(CuTest *tc)
{
    Parser *parser = get_instance();
    read_PAPI_events_from_file(parser, "pluginConf");
    CuAssertTrue(tc, parser->metrics_count == 3);
}

void TestParser_read_PAPI_preset_events_from_file(CuTest *tc)
{
    Parser *parser = get_instance();
    read_PAPI_events_from_file(parser, "pluginConf_all");
    CuAssertTrue(tc, parser->metrics_count == 1);
    CuAssertStrEquals(tc, parser->events[0], "ALL_PRESETS");
}

CuSuite* CuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestParser_read_PAPI_one_event_from_line);
	SUITE_ADD_TEST(suite, TestParser_read_PAPI_two_events_from_line);
	SUITE_ADD_TEST(suite, TestParser_read_PAPI_events_from_line_with_spaces);

	SUITE_ADD_TEST(suite, TestParser_read_PAPI_events_from_file);
	SUITE_ADD_TEST(suite, TestParser_read_PAPI_preset_events_from_file);

	return suite;
}
