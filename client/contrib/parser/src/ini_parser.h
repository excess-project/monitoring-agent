#ifndef INI_PARSER_H_
#define INI_PARSER_H_

typedef struct {
    char* keys[256];
    char* values[256];
    int size;
} mfp_data;

int mfp_parse(const char* filename);

char* mfp_get_value(const char* section, const char* key);

void mfp_set_value(const char* section, const char* key, const char* value);

void mfp_get_data(const char* section, mfp_data* data);

#endif
