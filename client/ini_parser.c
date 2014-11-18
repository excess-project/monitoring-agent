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
        log_error("parse(const char*, configuration) Can't load %s", filename);
        return 0;
    } else if (error) { // error
        //log_error("parse(const char*, configuration) bad config file; first error on line %d", error);
        return 0;
    }
    return 1;
}
