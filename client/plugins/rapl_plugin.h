#ifndef RAPL_PLUGIN_H_
#define RAPL_PLUGIN_H_

#include <papi.h>

#define MAX_RAPL_EVENTS 64

typedef struct RAPL_Plugin_t RAPL_Plugin;

struct RAPL_Plugin_t
{
    char events[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
    float *values;
    int data_types[MAX_RAPL_EVENTS];
    int num_events;
};

int get_available_events(RAPL_Plugin *rapl);

int get_rapl_component_id();

#endif
