#ifndef PAPI_PLUGIN_H_
#define PAPI_PLUGIN_H_

#include <papi.h>

typedef struct PAPI_Plugin_t PAPI_Plugin;

struct PAPI_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    long long values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

void read_available_named_events(PAPI_Plugin *papi, char **events, int num_events);

#endif
