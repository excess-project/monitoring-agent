#ifndef LIKWID_PLUGIN_H_
#define LIKWID_PLUGIN_H_

#include <cpuid.h>

#define MAX_NUM_THREADS 263

typedef struct Likwid_Plugin_t Likwid_Plugin;

struct Likwid_Plugin_t
{
    int hasDRAM;
    int hasPP0;
    int hasPP1;
    char power_names[4][256]; // fix me
    float power_values[4]; // fix me
    char sockets[4][4][128]; // fix me
    char dram[4][4][128]; // fix me
    char PP0[4][4][128]; // fix me
    char PP1[4][4][128]; // fix me
    int numSockets;
};

char* get_processor_model(void);

int is_processor_supported(void);

void check_processor(Likwid_Plugin *likwid);

void get_power_info(Likwid_Plugin *likwid);

void get_power_data(Likwid_Plugin *likwid, int duration_in_sec);

#endif
