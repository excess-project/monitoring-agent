#ifndef INI_PARSER_H_
#define INI_PARSER_H_

typedef struct {
    const char* hostname;
    const char* update_interval;
    const char* update_config;
} configuration;

int parse(const char* filename, configuration *config);

#endif
