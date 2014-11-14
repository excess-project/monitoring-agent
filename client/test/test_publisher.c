#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"
#include "../publisher.h"


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

void Test_successful_publish(CuTest *tc)
{
    const char *url = "http://localhost:3000";
    Message messages[1] = {
        {.sender = "fe.excess-project.eu" }
    };
    int retval = publish(url, messages);
    CuAssertTrue(tc, retval == 1);
}

CuSuite* CuGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, Test_data);
    SUITE_ADD_TEST(suite, Test_complex_message);
    SUITE_ADD_TEST(suite, Test_message);
    SUITE_ADD_TEST(suite, Test_successful_publish);

    return suite;
}
