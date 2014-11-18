#ifndef PARSER_H_
#define PARSER_H_

#include <papi.h>

typedef struct Parser_t Parser;

struct Parser_t
{
    char *events[PAPI_MAX_PRESET_EVENTS];
    int metrics_count;
};

Parser* get_instance();

void read_PAPI_events_from_file(Parser *parser, char *filename);

void read_PAPI_events_from_line(Parser *parser, char *line);

#endif
