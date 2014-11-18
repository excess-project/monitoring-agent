#ifndef INI_PARSER_H_
#define INI_PARSER_H_

typedef struct {
    const char* name;
    const char* email;
} configuration;

int parse(const char* filename, configuration *config);

#endif
