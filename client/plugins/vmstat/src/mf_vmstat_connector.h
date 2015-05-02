#ifndef _MF_VMSTAT_CONNECTOR_H_
#define _MF_VMSTAT_CONNECTOR_H_

#include <stddef.h>

typedef struct vmstat_plugin_t vmstat_plugin;

struct vmstat_plugin_t
{
    char* events[128];
    long long values[128];
    int num_events;
};

void mf_vmstat_read(
    vmstat_plugin* vmstat,
    char **named_events,
    size_t num_events
);

void mf_vmstat_print(vmstat_plugin* vmstat);

const char* mf_vmstat_tojson(vmstat_plugin* vmstat);

#endif
