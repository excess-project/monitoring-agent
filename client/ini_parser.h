#ifndef INI_PARSER_H_
#define INI_PARSER_H_

typedef struct {
    const char* hostname;
    const char* update_interval;
    const char* update_config;
} generic;

typedef struct {
    const char* default_timing;
    const char* papi;
    const char* rapl;
    const char* likwid;
    const char* mem_info;
    const char* hw_power;
} timings;

int parse_generic(const char* filename, generic *config);

int parse_timings(const char* filename, timings *config);

#endif
