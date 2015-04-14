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
#include "../src/publisher.h"

static const char *SERVER = "http://localhost:3000/executions";

static char *URL;

void Test_data(CuTest *tc)
{
    Data *data = malloc(sizeof(Data));
    data->key = malloc(sizeof(char) * 64);
    data->value = malloc(sizeof(char) * 64);
    strcpy(data->key, "key");
    strcpy(data->value, "value");
    CuAssertStrEquals(tc, "key", data->key);
    CuAssertStrEquals(tc, "value", data->value);
    free(data);
}

void Test_message(CuTest *tc)
{
    Message *message = malloc(sizeof(Message));
    message->sender = malloc(sizeof(char) * 64);
    strcpy(message->sender, "fe.excess-project.eu");
    CuAssertStrEquals(tc, "fe.excess-project.eu", message->sender);
    free(message);
}

void Test_complex_message(CuTest *tc)
{
    Message *message = malloc(sizeof(Message));
    message->sender = malloc(sizeof(char) * 64);
    strcpy(message->sender, "fe.excess-project.eu");

    message->data = malloc(sizeof(Data) * 2);
    message->data[0].key = "mem_avail";
    message->data[0].value = "50";
    message->data[1].key = "mem_used";
    message->data[1].value = "80";

    CuAssertStrEquals(tc, "mem_avail", message->data[0].key);
    CuAssertStrEquals(tc, "50", message->data[0].value);
    CuAssertStrEquals(tc, "mem_used", message->data[1].key);
    CuAssertStrEquals(tc, "80", message->data[1].value);

    free(message->data);
    free(message);
}

void Test_publish_json(CuTest *tc)
{
    char *message = "{ \"Start_date\":123, \"Username\":\"hopped\" }";
    int retval = publish_json(URL, message);
    CuAssertTrue(tc, retval == 1);
}

void Test_publish_json_with_empty_URL(CuTest *tc)
{
    char *null_URL = NULL;
    char *message = "{ \"Start_date\":123, \"Username\":\"hopped\" }";
    int retval = publish_json(null_URL, message);
    CuAssertTrue(tc, retval == 0);

    char *empty_URL = "";
    retval = publish_json(empty_URL, message);
    CuAssertTrue(tc, retval == 0);
}

void Test_publish_json_with_empty_message(CuTest *tc)
{
    char *null_message = NULL;
    int retval = publish_json(URL, null_message);
    CuAssertTrue(tc, retval == 0);

    char *empty_message = "";
    retval = publish_json(URL, empty_message);
    CuAssertTrue(tc, retval == 0);
}

void Test_successful_publish(CuTest *tc)
{
    Message messages[1] = {
        {.sender = "fe.excess-project.eu" }
    };
    int retval = publish(URL, messages);
    CuAssertTrue(tc, retval == 1);
}

void set_default_query(char *query)
{
    char *hostname = "localhost";
    char *description = "test";
    char *start_date = "Tu 18 Nov 2014 13:08:47 CET";
    char *username = "hpcmfagent";
	sprintf(query,
	    "{\"Name\":\"%s\", \"Description\":\"%s\", \"Start_date\":\"%s\", \"Username\":\"%s\"}",
	    hostname, description, start_date, username
	);
}

void Test_get_execution_id(CuTest *tc)
{
    char *query = malloc(sizeof(char) * 256);
    set_default_query(query);
    char *id = get_execution_id(SERVER, query);
    CuAssertTrue(tc, id != NULL && strlen(id) == 20);
    free(query);
}

void startup()
{
    char *query = malloc(sizeof(char) * 256);
    set_default_query(query);
    URL = malloc(sizeof(char) * 256);
    strcpy(URL, SERVER);
    strcat(URL, "/");
    strcat(URL, get_execution_id(SERVER, query));
    free(query);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    startup();

    SUITE_ADD_TEST(suite, Test_data);
    SUITE_ADD_TEST(suite, Test_complex_message);
    SUITE_ADD_TEST(suite, Test_message);
    SUITE_ADD_TEST(suite, Test_publish_json);
    SUITE_ADD_TEST(suite, Test_publish_json_with_empty_URL);
    SUITE_ADD_TEST(suite, Test_publish_json_with_empty_message);
    SUITE_ADD_TEST(suite, Test_successful_publish);
    SUITE_ADD_TEST(suite, Test_get_execution_id);

    //shutdown_curl();

    return suite;
}
