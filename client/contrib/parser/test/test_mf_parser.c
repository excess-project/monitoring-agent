/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../../libs/CuTest/CuTest.h"
#include "mf_parser.h"

void Test_parse(CuTest *tc)
{
    int retval = mfp_parse("config.ini");
    CuAssertTrue(tc, retval);
}

void Test_parse_non_existing_file(CuTest *tc)
{
    int retval = mfp_parse("not_existing.ini");
    CuAssertTrue(tc, !retval);
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

void Test_set_value_for_non_existing_section(CuTest *tc)
{
    mfp_data *data = malloc(sizeof(mfp_data));

    mfp_set_value("new_section", "foo", "bar");
    mfp_get_data("new_section", data);
    CuAssertStrEquals(tc, "foo", data->keys[0]);
    CuAssertStrEquals(tc, "bar", data->values[0]);

    free(data);
}

void Test_set_value_for_existing_key(CuTest *tc)
{
    mfp_data *data = malloc(sizeof(mfp_data));

    mfp_parse("config.ini");
    char* value = mfp_get_value("plugins", "papi");
    CuAssertStrEquals(tc, "off", value);

    mfp_set_value("plugins", "papi", "on");
    value = mfp_get_value("plugins", "papi");
    CuAssertStrEquals(tc, "on", value);

    free(data);
}

void Test_get_data_filtered(CuTest *tc)
{
    mfp_data *data = malloc(sizeof(mfp_data));

    mfp_parse("config.ini");
    mfp_get_data_filtered_by_value("papi", data, "on");
    CuAssertTrue(tc, data->size == 1);

    free(data);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    // mfp_parse
    SUITE_ADD_TEST(suite, Test_parse);
    SUITE_ADD_TEST(suite, Test_parse_non_existing_file);

    // mfp_set
    SUITE_ADD_TEST(suite, Test_set_value_for_non_existing_section);
    SUITE_ADD_TEST(suite, Test_set_value_for_existing_key);

    // mfp_get
    SUITE_ADD_TEST(suite, Test_get_section_value);
    SUITE_ADD_TEST(suite, Test_get_non_existing_section);
    SUITE_ADD_TEST(suite, Test_get_non_existing_value);

    // mfp_keys_values
    SUITE_ADD_TEST(suite, Test_keys_values_check_plugins_size);
    SUITE_ADD_TEST(suite, Test_keys_values_check_key_of_timings);
    SUITE_ADD_TEST(suite, Test_keys_values_check_value_of_timings);

    // mfp_get_data
    SUITE_ADD_TEST(suite, Test_get_data_filtered);

    return suite;
}
