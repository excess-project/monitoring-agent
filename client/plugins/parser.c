#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "parser.h"

Parser* get_instance()
{
    static Parser *instance = NULL;

    if (instance == NULL) {
        instance = malloc(sizeof(Parser));
    }

    return instance;
}

void read_PAPI_events_from_line(Parser *parser, char *line)
{
    int j, n;
    char *ptr = line;
    parser->metrics_count = 0;
    char metric[256];

    while (sscanf(ptr, "%s %n", metric, &n) == 1) {
        parser->events[parser->metrics_count] = malloc(strlen(metric) + 1);
        strcpy(parser->events[parser->metrics_count], metric);
        ptr += n;
        parser->metrics_count++;
    }
}

void read_PAPI_events_from_file(Parser *parser, char *filename)
{
    char buffer[4096];
    char *prefix = "#";
    char *plugin_name = "#PAPI";
    char *strEvents = "Events:";
    int flag = 0;

    FILE *fp;
    fp = fopen(filename, "r");
    assert(fp != NULL);

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strncmp(prefix, buffer, strlen(prefix)) == 0) {
            if (strncmp(plugin_name, buffer, strlen(plugin_name)) == 0) {
                flag = 1;
            } else {
                flag = 0;
            }
            continue;
        }
        if (flag) {
            if (strncmp(strEvents, buffer, strlen(strEvents)) == 0) {
                memmove(buffer, buffer + strlen(strEvents), strlen(buffer));
                while (isspace(*buffer)) {
                    memmove(buffer, buffer + 1, strlen(buffer));
                }
                read_PAPI_events_from_line(parser, buffer);
            }
        }
    }

    fclose(fp);
}
