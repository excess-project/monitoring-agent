#include "likwid_plugin.h"

#include <power.h>
#include <stdlib.h>
#include <cpuid.h>
#include <accessClient.h>


int is_supported()
{
    if (cpuid_init() == EXIT_FAILURE) {
        printf("likwid-rapl.c: Unsupported processor!\n");
        return 0;
    }

    return 1;
}

char* get_processor_model(void)
{
    is_supported();
    return cpuid_info.name;
}

int is_processor_supported(void)
{
    if ((cpuid_info.model != SANDYBRIDGE) &&
        (cpuid_info.model != SANDYBRIDGE_EP) &&
        (cpuid_info.model != IVYBRIDGE) &&
        (cpuid_info.model != IVYBRIDGE_EP) &&
        (cpuid_info.model != HASWELL) &&
        (cpuid_info.model != HASWELL_M1) &&
        (cpuid_info.model != HASWELL_M2) &&
        (cpuid_info.model != HASWELL_EX) &&
        (cpuid_info.model != ATOM_SILVERMONT_C) &&
        (cpuid_info.model != ATOM_SILVERMONT_E) &&
        (cpuid_info.model != ATOM_SILVERMONT_F1) &&
        (cpuid_info.model != ATOM_SILVERMONT_F2) &&
        (cpuid_info.model != ATOM_SILVERMONT_F3)) {
        fprintf(stderr, "RAPL not supported on this processor!\n");
        return 0;
    }

    return 1;
}

void check_processor(Likwid_Plugin *likwid)
{
    if ((cpuid_info.model == SANDYBRIDGE_EP) ||
        (cpuid_info.model == IVYBRIDGE_EP) ||
        (cpuid_info.model == HASWELL_EX) ||
        (cpuid_info.model == HASWELL)) {
        likwid->hasDRAM = 1;
    }
    if ((cpuid_info.model == SANDYBRIDGE_EP) ||
        (cpuid_info.model == SANDYBRIDGE) ||
        (cpuid_info.model == IVYBRIDGE_EP) ||
        (cpuid_info.model == IVYBRIDGE) ||
        (cpuid_info.model == HASWELL) ||
        (cpuid_info.model == ATOM_SILVERMONT_E) ||
        (cpuid_info.model == ATOM_SILVERMONT_F1) ||
        (cpuid_info.model == ATOM_SILVERMONT_F2) ||
        (cpuid_info.model == ATOM_SILVERMONT_F3)) {
        likwid->hasPP0 = 1;
    }
    if ((cpuid_info.model == HASWELL) ||
        (cpuid_info.model == SANDYBRIDGE) ||
        (cpuid_info.model == IVYBRIDGE)) {
        likwid->hasPP1 = 1;
    }
}

/*
 *     typedef struct {
        double baseFrequency;
        double minFrequency;
        TurboBoost turbo;
        double powerUnit;
        double energyUnit;
        double timeUnit;
        double tdp;
        double minPower;
        double maxPower;
        double maxTimeWindow;
    } PowerInfo;
 */
void get_power_info(Likwid_Plugin *likwid)
{
    int socket_fd = -1;
    accessClient_setaccessmode(1);
    accessClient_init(&socket_fd);
    msr_init(socket_fd);

    power_init(0);

    int size = sizeof(likwid->power_names[0]);
    strncpy(likwid->power_names[0], "TDP", size);
    strncpy(likwid->power_names[1], "Minimum Power", size);
    strncpy(likwid->power_names[2], "Maximum Power", size);
    strncpy(likwid->power_names[3], "Maximum Time Window", size);
    likwid->power_values[0] = power_info.tdp;
    printf("%g\n\n", power_info.tdp);
    likwid->power_values[1] = power_info.minPower;
    likwid->power_values[2] = power_info.maxPower;
    likwid->power_values[3] = power_info.maxTimeWindow;
}
