#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../../libs/CuTest/CuTest.h"
#include "../src/ini_parser.h"

void Test_parse(CuTest *tc)
{
    int retval = mfp_parse("config.ini");
    CuAssertTrue(tc, retval);
}

void Test_get_section_value(CuTest *tc)
{
    mfp_parse("config.ini");
    char* server_name = mfp_get_value("generic", "server");
    CuAssertStrEquals(tc, "http://141.58.0.2:3000/executions/", server_name);
}

void Test_get_non_existing_section(CuTest *tc)
{
    mfp_parse("config.ini");
    char* lottery_numbers = mfp_get_value("lottery", "numbers");
    CuAssertTrue(tc, (lottery_numbers == NULL || strlen(lottery_numbers) == 0));
}

void Test_get_non_existing_value(CuTest *tc)
{
    mfp_parse("config.ini");
    char* lottery_numbers = mfp_get_value("generic", "numbers");
    CuAssertTrue(tc, (lottery_numbers == NULL || strlen(lottery_numbers) == 0));
}

void Test_keys_values_check_plugins_size(CuTest *tc)
{
    mfp_data *data = malloc(sizeof(mfp_data));

    mfp_parse("config.ini");
    mfp_get_data("plugins", data);
    CuAssertTrue(tc, 5 == data->size);

    free(data);
}

void Test_keys_values_check_key_of_timings(CuTest *tc)
{
    int key_found = 0;
    mfp_data *data = malloc(sizeof(mfp_data));

    mfp_parse("config.ini");
    mfp_get_data("timings", data);
    for (int i = 0; i < data->size; ++i) {
        if (strcmp("likwid", data->keys[i]) == 0) {
            key_found = 1;
        }
    }
    CuAssertTrue(tc, key_found);

    free(data);
}

void Test_keys_values_check_value_of_timings(CuTest *tc)
{
    int key_value_pair_found = 0;
    mfp_data *data = malloc(sizeof(mfp_data));

    mfp_parse("config.ini");
    mfp_get_data("timings", data);
    for (int i = 0; i < data->size; ++i) {
        if (strcmp("likwid", data->keys[i]) == 0) {
            if (strcmp("1000000000ns", data->values[i]) == 0) {
                key_value_pair_found = 1;
            }
        }
    }
    CuAssertTrue(tc, key_value_pair_found);

    free(data);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    // mfp_parse
    SUITE_ADD_TEST(suite, Test_parse);

    // mfp_get
    SUITE_ADD_TEST(suite, Test_get_section_value);
    SUITE_ADD_TEST(suite, Test_get_non_existing_section);
    SUITE_ADD_TEST(suite, Test_get_non_existing_value);

    // mfp_keys_values
    SUITE_ADD_TEST(suite, Test_keys_values_check_plugins_size);
    SUITE_ADD_TEST(suite, Test_keys_values_check_key_of_timings);
    SUITE_ADD_TEST(suite, Test_keys_values_check_value_of_timings);

    return suite;
}