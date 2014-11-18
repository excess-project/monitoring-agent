#include "ini.h"
#include "ini_parser.h"
#include "debug.h"

#define MATCH_SECTION(s) strcmp(section, s) == 0
#define MATCH_KEY(n) strcmp(name, n) == 0


static int handle_generic(void* user, const char* section, const char* name, const char* value)
{
    generic* pconfig = (generic*) user;

    if (!MATCH_SECTION("generic")) {
        return 0;
    }

    if (MATCH_KEY("hostname")) {
        pconfig->hostname = strdup(value);
    } else if (MATCH_KEY("publish_data_interval")) {
        pconfig->update_interval = strdup(value);
    } else if (MATCH_KEY("update_configuration")) {
        pconfig->update_config = strdup(value);
    } else {
        log_error("handler_generic(..) Found an unknown entity: '%s'", name);
    }

    return 1;
}


int parse_generic(const char* filename, generic *config)
{
    int error = ini_parse(filename, handle_generic, config);
    if (error < 0) {
        log_error("parse_generic(const char*, generic) Can't load %s", filename);
        return 0;
    } else if (error) { // error
        //log_error("parse(const char*, configuration) bad config file; first error on line %d", error);
        return 0;
    }

    return 1;
}


static int handle_timings(void* user, const char* section, const char* name, const char* value)
{
    timings* pconfig = (timings*) user;
    
    if (!MATCH_SECTION("timings")) {
        return 0;
    }
    
    if (MATCH_KEY("default")) {
        pconfig->default_timing = strdup(value);
    } else if (MATCH_KEY("papi")) {
        pconfig->papi = strdup(value);        
    } else if (MATCH_KEY("rapl")) {
        pconfig->rapl = strdup(value);
    } else if (MATCH_KEY("mem_info")) {
        pconfig->mem_info = strdup(value);
    } else if (MATCH_KEY("likwid")) {
        pconfig->likwid = strdup(value);
    } else if (MATCH_KEY("hw_power")) {
        pconfig->hw_power = strdup(value);                        
    } else {
        log_error("handler_timings(..) Found an unknown entity: '%s'", name);
    }
    
    return 1;
}


int parse_timings(const char* filename, timings *config)
{
    config->papi = NULL;
    config->rapl = NULL;
    config->hw_power = NULL;
    config->likwid = NULL;
    config->mem_info = NULL;
    
    int error = ini_parse(filename, handle_timings, config);
    
    if (config->papi == NULL || strlen(config->papi) == '\0') {
        config->papi = config->default_timing;
    }
    if (config->rapl == NULL || strlen(config->rapl) == '\0') {
        config->rapl = config->default_timing;
    }
    if (config->hw_power == NULL || strlen(config->hw_power) == '\0') {
        config->hw_power = config->default_timing;
    }
    if (config->likwid == NULL || strlen(config->likwid) == '\0') {
        config->likwid = config->default_timing;
    }
    if (config->mem_info == NULL || strlen(config->mem_info) == '\0') {
        config->mem_info = config->default_timing;
    }

    if (error < 0) {
        log_error("parse_timings(const char*, timings) Can't load %s", filename);
        return 0;
    } else if (error) { // error
        //log_error("parse(const char*, configuration) bad config file; first error on line %d", error);
        return 0;
    }

    return 1;
}

static int handle_plugins(void* user, const char* section, const char* name, const char* value)
{
    plugins* pconfig = (plugins*) user;

    if (!MATCH_SECTION("plugins")) {
        return 0;
    }

    if (MATCH_KEY("papi")) {
        pconfig->papi = strdup(value);
    } else if (MATCH_KEY("rapl")) {
        pconfig->rapl = strdup(value);
    } else if (MATCH_KEY("mem_info")) {
        pconfig->mem_info = strdup(value);
    } else if (MATCH_KEY("likwid")) {
        pconfig->likwid = strdup(value);
    } else if (MATCH_KEY("hw_power")) {
        pconfig->hw_power = strdup(value);                       
    } else {
        log_error("handler_plugins(..) Found an unknown entity: '%s'", name);
    }

    return 1;
}


int parse_plugins(const char* filename, plugins *config)
{
    int error = ini_parse(filename, handle_plugins, config);
    if (error < 0) {
        log_error("parse_plugins(const char*, plugins) Can't load %s", filename);
        return 0;
    } else if (error) { // error
        //log_error("parse(const char*, configuration) bad config file; first error on line %d", error);
        return 0;
    }

    return 1;
}


static int handle_plugin(void* user, const char* section, const char* name, const char* value)
{
    plugin* pconfig = (plugin*) user;

    if (!MATCH_SECTION(pconfig->name)) {
        return 0;
    }

    pconfig->events[pconfig->size] = strdup(name);
    pconfig->values[pconfig->size] = strdup(value);
    ++pconfig->size;

    return 1;
}


int parse_plugin(const char* filename, const char* plugin_name, plugin *config)
{
    config->name = plugin_name;
    config->size = 0;

    int error = ini_parse(filename, handle_plugin, config);
    if (error < 0) {
        log_error("parse_plugin(const char*, const char*, plugin) Can't load %s", filename);
        return 0;
    } else if (error) { // error
        //log_error("parse(const char*, configuration) bad config file; first error on line %d", error);
        return 0;
    }

    return 1;
}
