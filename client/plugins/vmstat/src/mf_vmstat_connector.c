#include "mf_vmstat_connector.h"
#include "mf_debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void
init(vmstat_plugin* vmstat)
{
    if (vmstat == NULL) {
        vmstat = malloc(sizeof(vmstat_plugin));
    }
    memset(vmstat, 0, sizeof(vmstat_plugin));
    vmstat->num_events = 0;
}

void
mf_vmstat_read(vmstat_plugin* vmstat, char **named_events, size_t num_events)
{
    char key[128];
    long long value = 0;

    init(vmstat);

    FILE *fp = fopen("/proc/vmstat", "r");
    while (fscanf(fp, "%s %lld", key, &value) == 2) {
        int i;
        for (i = 0; i != num_events; ++i) {
            debug("Key: %s and event %s", key, named_events[i]);
            if (strcmp(key, named_events[i]) == 0) {
                vmstat->events[vmstat->num_events] = malloc(sizeof(char) * 256);
                strcpy(vmstat->events[vmstat->num_events], key);
                vmstat->values[vmstat->num_events] = value;
                vmstat->num_events++;
                break;
            }
        }
    }
    fclose(fp);
}

void
mf_vmstat_print(vmstat_plugin* vmstat)
{
    int i = 0;
    for (i = 0; i != vmstat->num_events; ++i) {
        printf("%s\t%lld\n", vmstat->events[i], vmstat->values[i]);
    }
}

const char*
mf_vmstat_tojson(vmstat_plugin* vmstat)
{
    int i;
    char* json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"statistics\"");

    char* metric = malloc(512 * sizeof(char));
    for (i = 0; i < vmstat->num_events; ++i) {
        sprintf(metric, ",\"%s\":%lld", vmstat->events[i], vmstat->values[i]);
        strcat(json, metric);
    }
    free(metric);

    return json;
}