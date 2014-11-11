#ifndef LIKWID_PLUGIN_H_
#define LIKWID_PLUGIN_H_

#include <cpuid.h>

typedef struct Likwid_Plugin_t Likwid_Plugin;

struct Likwid_Plugin_t
{
    int hasDRAM ;
    int hasPP0;
    int hasPP1;
    char power_names[4][128];
    float power_values[4];
};

char* get_processor_model(void);

int is_processor_supported(void);

void check_processor(Likwid_Plugin *likwid);

void get_power_info(Likwid_Plugin *likwid);

#endif
