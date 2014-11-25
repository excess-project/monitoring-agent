#include <stdlib.h>
#include <apr_strings.h>
#include <apr_general.h>

#include "../libs/ini/ini.h"
#include "../../../debug.h"
#include "ini_parser.h"

#define MATCH_SECTION(s) strcmp(section, s) == 0
#define MATCH_KEY(n) strcmp(name, n) == 0

static apr_pool_t *mp;
static apr_hash_t *plugins;
static int ht_initialized = 0;

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


static void init_ht()
{
    if (ht_initialized) {
        return;
    }

    apr_initialize();
    apr_pool_create(&mp, NULL);
    plugins = apr_hash_make(mp);
    ht_initialized = 1;

    /* FIXME
    apr_pool_destroy(mp);
    apr_terminate();
    */
}


static int handle_plugins(void* user, const char* section, const char* name, const char* value)
{
    if (!MATCH_SECTION("plugins")) {
        return 1;
    }

    init_ht();

    debug("handle_plugins -- name: %s, status: %s", name, value);

    apr_hash_set(plugins,
        apr_pstrdup(mp, name),
        APR_HASH_KEY_STRING,
        apr_pstrdup(mp, value)
    );

    return 1;
}

typedef struct {
  int nothing;
} empty;

int parse_plugins(const char* filename)
{
    empty config;
    int error = ini_parse(filename, handle_plugins, &config);
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


int parse_plugin(const char* filename, const char* plugin_name, config_plugin *config)
{
    strcpy(config->name, plugin_name);
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

int is_enabled(const char* plugin_name)
{
    const char *result = apr_hash_get(plugins, plugin_name, APR_HASH_KEY_STRING);
    if (strcmp(result, "on") == 0) {
        return 1;
    }
    return 0;
}


void read_plugins(char** names)
{
    apr_hash_index_t *hi;
    for (hi = apr_hash_first(NULL, plugins); hi; hi = apr_hash_next(hi)) {
        const char *k;
        const char *v;
        
        apr_hash_this(hi, (const void**)&k, NULL, (void**)&v);
        printf("ht iteration: key=%s, val=%s\n", k, v);
    }
}


int num_plugins()
{
    return apr_hash_count(plugins);
}

int num_active_plugins()
{
    int retval = 0;

    apr_hash_index_t *hi;
    for (hi = apr_hash_first(NULL, plugins); hi; hi = apr_hash_next(hi)) {
        const char *k;
        const char *v;

        apr_hash_this(hi, (const void**)&k, NULL, (void**)&v);
        if (strcmp("on", v)) {
            ++retval;
        }
    }

    return retval;
}