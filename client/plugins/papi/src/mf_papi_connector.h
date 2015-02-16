#ifndef _MF_PAPI_CONNECTOR_H_
#define _MF_PAPI_CONNECTOR_H_

#include <papi.h>
#include <time.h>

typedef struct PAPI_Plugin_t PAPI_Plugin;

struct PAPI_Plugin_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    long long values[PAPI_MAX_PRESET_EVENTS];
    int num_events;
};

void mf_papi_init(char **named_events, size_t num_events);

void mf_papi_profile(struct timespec profile_interval);

void mf_papi_read(PAPI_Plugin *papi, char **events);

void mf_papi_shutdown();

#endif