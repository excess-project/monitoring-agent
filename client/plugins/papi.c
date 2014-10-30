#include <stdlib.h>
#include <papi.h>

#include "../util.h"
#include "../plugin_manager.h"
#include "../excess_main.h"

const char *ALL_PRESETS = "ALL_PRESETS";

void read_metric_info(
        int EventSet,
        PAPI_event_info_t metric,
        char **collected_events,
        long long *collected_values,
        int *num_events)
{
    PAPI_start(EventSet);

    // START RETRIEVE DATA
    long long value;
    PAPI_read(EventSet, &value);

    collected_events[*num_events] = malloc(strlen(metric.symbol) + 1);
    strcpy(collected_events[*num_events], metric.symbol);
    collected_values[*num_events] = value;
    *num_events += 1;
    // END RETRIEVE DATA

    PAPI_stop(EventSet, &value);
    PAPI_remove_event(EventSet, (int) metric.event_code);
}

void retrieve_all_available_presets(
        int MASK,
        char **collected_events,
        long long *collected_values,
        int *num_events)
{
    int i = 0;
    int EventSet = PAPI_NULL;
    PAPI_event_info_t metric;
    *num_events = 0;

    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);

    for (i = 0; i < PAPI_MAX_PRESET_EVENTS; ++i) {
        if (PAPI_get_event_info(MASK | i, &metric) != PAPI_OK) {
            continue;
        }

        if (PAPI_add_event(EventSet, (int) metric.event_code) == PAPI_OK) {
            read_metric_info(EventSet, metric, collected_events,
                    collected_values, num_events);
        }
    }

    PAPI_destroy_eventset(&EventSet);
    PAPI_shutdown();
}

void retrieve_all_available_events_for(
        const char *event_group,
        char **collected_events,
        long long *collected_values,
        int *num_events)
{
    if (strncmp(ALL_PRESETS, event_group, strlen(ALL_PRESETS)) == 0) {
        retrieve_all_available_presets(PAPI_PRESET_MASK, collected_events,
                collected_values, num_events);
    }
}

char* to_JSON(
        char *collected_events[],
        long long collected_values[],
        int num_events)
{
    int i;
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"papi\"");

    char *single_metric = malloc(512 * sizeof(char));
    for (i = 0; i < num_events; ++i) {
        sprintf(single_metric, ",\"%s\":%lld", collected_events[i],
                collected_values[i]);
        strcat(json, single_metric);
    }
    free(single_metric);

    return json;
}

static metric papi_hook()
{
    if (running) {
        metric resMetric = malloc(sizeof(metric_t));
        resMetric->msg = malloc(4096 * sizeof(char));

        int clk_id = CLOCK_REALTIME;
        clock_gettime(clk_id, &resMetric->timestamp);

        // START
        int num_events;
        const char *event_group = "ALL_PRESETS";
        char *collected_events[PAPI_MAX_PRESET_EVENTS];
        long long collected_values[PAPI_MAX_PRESET_EVENTS];
        retrieve_all_available_events_for(event_group, collected_events,
                collected_values, &num_events);

        strcpy(resMetric->msg,
                to_JSON(collected_events, collected_values, num_events));
        // END

        return resMetric;
    } else {
        return NULL;
    }
}

extern int init_papi(PluginManager *pm)
{
    PluginManager_register_hook(pm, "papi", papi_hook);

    return 1;
}
