#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <papi.h>

#include "papi_plugin.h"

const char *ALL_PRESETS = "ALL_PRESETS";


void read_named_metric_info(PAPI_Plugin *papi, int EventSet,
        char *metric_name, int event_code)
{
    PAPI_start(EventSet);

    // START RETRIEVE DATA
    long long value;
    PAPI_read(EventSet, &value);

    papi->events[papi->num_events] = malloc(PAPI_MAX_STR_LEN + 1);
    strcpy(papi->events[papi->num_events], metric_name);
    papi->values[papi->num_events] = value;
    papi->num_events++;
    // END RETRIEVE DATA

    PAPI_stop(EventSet, &value);
    PAPI_remove_event(EventSet, event_code);
}

void read_metric_info(PAPI_Plugin *papi, int EventSet, PAPI_event_info_t metric)
{
    read_named_metric_info(papi, EventSet,
            metric.symbol, (int) metric.event_code);
}

void read_all_available_presets(PAPI_Plugin *papi, int EventSet, int MASK)
{
    int i = 0;
    PAPI_event_info_t metric;

    for (i = 0; i < PAPI_MAX_PRESET_EVENTS; ++i) {
        if (PAPI_get_event_info(MASK | i, &metric) != PAPI_OK) {
            continue;
        }

        if (PAPI_add_event(EventSet, (int) metric.event_code) == PAPI_OK) {
            read_metric_info(papi, EventSet, metric);
        }
    }
}

int is_preset_event(char *event_name)
{
    return (strncmp(ALL_PRESETS, event_name, strlen(ALL_PRESETS)) == 0) ? 1 : 0;
}

void read_available_named_events(PAPI_Plugin *papi, char **events, int num_events)
{
    int i = 0;
    int event_code = PAPI_NULL;
    int EventSet = PAPI_NULL;

    papi->num_events = 0;

    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);

    for (i = 0; i < num_events; ++i) {
        if (PAPI_add_named_event(EventSet, events[i]) == PAPI_OK) {
            read_named_metric_info(papi, EventSet, events[i], event_code);
        } else if (is_preset_event(events[i])) {
            read_all_available_presets(papi, EventSet, PAPI_PRESET_MASK);
        } else {
            // handle error
        }
    }

    PAPI_destroy_eventset(&EventSet);
    PAPI_shutdown();
}
