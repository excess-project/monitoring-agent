#ifndef INI_PARSER_H_
#define INI_PARSER_H_

typedef struct {
    char* server;
} config_generic;

typedef struct {
    long default_timing;
    long update_interval;
    long update_config;
    long papi;
    long rapl;
    long likwid;
    long mem_info;
    long hw_power;
    // FIXME: just for compatibility (likely to be removed)
    long long timings[256];
    int num_plugins;
} config_timings;

typedef struct {
    char* papi;
    char* rapl;
    char* likwid;
    char* mem_info;
    char* hw_power;
} config_plugins;

typedef struct {
    char* name;
    char* events[256];
    int size;
} config_plugin;

int parse_generic(const char* filename, config_generic *config);

int parse_timings(const char* filename, config_timings *config);

int parse_plugins(const char* filename, config_plugins *config);

int parse_plugin(const char* filename, char* plugin_name, config_plugin *config);

#endif
