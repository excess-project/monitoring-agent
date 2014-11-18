#include "ini.h"
#include "ini_parser.h"
#include "debug.h"

static int handler(void* user, const char* section, const char* name, const char* value)
{
    return 1;
}


int parse(const char* filename, configuration *config)
{
    if (ini_parse(filename, handler, &config) < 0) {
        log_error("parse(const char*, configuration) %s %s", "Can't load", filename);
        return 0;
    }
    
    return 1;
}
