#ifndef _MF_PAPI_CONNECTOR_H_
#define _MF_PAPI_CONNECTOR_H_

#include <papi.h>

typedef struct PAPI_Plugin_t PAPI_Plugin;

struct PAPI_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    long long values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

void read_counters(PAPI_Plugin *papi, char **events, size_t num_events);

#endif