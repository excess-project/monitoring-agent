#ifndef INI_PARSER_H_
#define INI_PARSER_H_

#include <apr.h>
#include <apr_hash.h>

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
    char name[256];
    char* events[256];
    int size;
} config_plugin;

int parse_generic(const char* filename, config_generic *config);

int parse_timings(const char* filename, config_timings *config);

int parse_plugins(const char* filename);

int parse_plugin(const char* filename, const char* plugin_name, config_plugin *config);

//------------------------------------------------------------------------------
// PLUGIN CONFIGURATION
//------------------------------------------------------------------------------

int is_enabled(const char* plugin_name);

void read_plugins(char** plugins);

int num_plugins();

int num_active_plugins();



#endif
