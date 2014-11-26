#include <apr.h>
#include <apr_hash.h>
#include <apr_strings.h>
#include <apr_general.h>
#include <stdlib.h>

#include "../libs/ini/ini.h"
#include "../../../debug.h"
#include "ini_parser.h"

#define MAKE_DUP(s) apr_pstrdup(mp, s)

static apr_pool_t *mp;
static apr_hash_t *ht_config;
static int ht_initialized = 0;

static void intialize_ht();
static int handle_parser(void*, const char*, const char*, const char*);

int mfp_parse(const char* filename)
{
    intialize_ht();

    // apr_hash_clear(ht_config);  // FIXME: concurrency issue
    int error = ini_parse(filename, handle_parser, ht_config);
    if (error < 0) {
        log_error("mfp_parse(const char*) Can't load %s", filename);
        return 0;
    }

    return 1;
}

void mfp_set_value(const char* section, const char* key, const char* value);

static int handle_parser(void* user, const char* section, const char* name, const char* value)
{
    mfp_set_value(section, name, value);
    return 1;
}

void mfp_set_value(const char* section, const char* key, const char* value)
{
    intialize_ht();

    apr_hash_t *ht_values = apr_hash_get(ht_config, section, APR_HASH_KEY_STRING);
    if (ht_values == NULL) {
        log_info("mfp_set_value(..) -- Created new hash_table for section: %s", section);
        ht_values = apr_hash_make(mp);
    }

    log_info("mfp_set_value(..) -- Set new values <%s,%s>", key, value);
    apr_hash_set(ht_values, MAKE_DUP(key), APR_HASH_KEY_STRING, MAKE_DUP(value));
    apr_hash_set(ht_config, MAKE_DUP(section), APR_HASH_KEY_STRING, ht_values);
}

static void intialize_ht()
{
    if (ht_initialized) {
        return;
    }

    apr_initialize();
    apr_pool_create(&mp, NULL);
    ht_config = apr_hash_make(mp);
    ht_initialized = 1;
}

char* mfp_get_value(const char* section, const char* key)
{
    intialize_ht();

    apr_hash_t *ht_values = apr_hash_get(ht_config, section, APR_HASH_KEY_STRING);
    if (ht_values == NULL) {
        log_error("mfp_get_value(const char*, const char*) Key does not exist: <%s:%s>", section, key);
        return '\0';
    }

    return (char*) apr_hash_get(ht_values, key, APR_HASH_KEY_STRING);
}

void mfp_get_data(const char* section, mfp_data* data)
{
    intialize_ht();

    apr_hash_index_t *ht_index;
    apr_hash_t *ht_section;
    data->size = 0;

    ht_section = apr_hash_get(ht_config, section, APR_HASH_KEY_STRING);
    if (ht_section == NULL) {
        log_error("mfp_get_data(const char*, mfp_data*) Section does not exist: %s", section);
        return;
    }

    for (ht_index = apr_hash_first(NULL, ht_section); ht_index; ht_index = apr_hash_next(ht_index)) {
        const char *key;
        const char *value;

        apr_hash_this(ht_index, (const void**)&key, NULL, (void**)&value);

        data->keys[data->size] = malloc(sizeof(char) * strlen(key));
        strcpy(data->keys[data->size], key);
        data->values[data->size] = malloc(sizeof(char) * strlen(value));
        strcpy(data->values[data->size], value);

        data->size++;
    }
}

/*
void shutdown()
{
    apr_pool_destroy(mp);
    apr_terminate();
}
*/