#include <stdlib.h>

#include "../libs/ini/ini.h"
#include "ini_parser.h"
#include "../../debug.h"

#define MATCH_SECTION(s) strcmp(section, s) == 0
#define MATCH_KEY(n) strcmp(name, n) == 0


static int handle_generic(void* user, const char* section, const char* name, const char* value)
{
    config_generic* pconfig = (config_generic*) user;

    if (!MATCH_SECTION("generic")) {
        return 1;
    }

    if (MATCH_KEY("server")) {
        pconfig->server = strdup(value);
    } else {
        log_error("handler_generic(..) Found an unknown entity: '%s'", name);
        return 0;
    }

    return 1;
}


int parse_generic(const char* filename, config_generic *config)
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
    config_timings* pconfig = (config_timings*) user;

    if (!MATCH_SECTION("timings")) {
        return 1;
    }

    if (MATCH_KEY("default")) {
        pconfig->default_timing = atoi(value);
    } else if (MATCH_KEY("publish_data_interval")) {
        pconfig->update_interval = atoi(value);
        pconfig->timings[0] = atoi(value);  // FIXME
    } else if (MATCH_KEY("update_configuration")) {
        pconfig->update_config = atoi(value);
        pconfig->timings[1] = atoi(value);  // FIXME
    } else if (MATCH_KEY("papi")) {
        pconfig->papi = atoi(value);
        pconfig->timings[pconfig->num_plugins] = atoi(value);  // FIXME
        ++pconfig->num_plugins;
    } else if (MATCH_KEY("rapl")) {
        pconfig->rapl = atoi(value);
        pconfig->timings[pconfig->num_plugins] = atoi(value);  // FIXME
        ++pconfig->num_plugins;
    } else if (MATCH_KEY("mem_info")) {
        pconfig->mem_info = atoi(value);
        pconfig->timings[pconfig->num_plugins] = atoi(value);  // FIXME
        ++pconfig->num_plugins;
    } else if (MATCH_KEY("likwid")) {
        pconfig->likwid = atoi(value);
        pconfig->timings[pconfig->num_plugins] = atoi(value);  // FIXME
        ++pconfig->num_plugins;
    } else if (MATCH_KEY("hw_power")) {
        pconfig->hw_power = atoi(value);
        pconfig->timings[pconfig->num_plugins] = atoi(value);  // FIXME
        ++pconfig->num_plugins;
    } else {
        log_error("handler_timings(..) Found an unknown entity: '%s'", name);
        return 0;
    }

    return 1;
}


int parse_timings(const char* filename, config_timings *config)
{
    config->num_plugins = 2; // send_data, update config
    int error = ini_parse(filename, handle_timings, config);
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
    config_plugins* pconfig = (config_plugins*) user;

    if (!MATCH_SECTION("plugins")) {
        return 1;
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
        return 0;
    }

    return 1;
}


int parse_plugins(const char* filename, config_plugins *config)
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
    config_plugin* pconfig = (config_plugin*) user;

    if (!MATCH_SECTION(pconfig->name)) {
        return 1;
    }

    if (strcmp("off", value) == 0) {
        return 1;
    }

    pconfig->events[pconfig->size] = strdup(name);
    ++pconfig->size;

    return 1;
}


int parse_plugin(const char* filename, char* plugin_name, config_plugin *config)
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
    } else if (config->size == 0) {
        return 0;
    }

    return 1;
}
