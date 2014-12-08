#include "likwid_plugin.h"

#include <power.h>
#include <stdlib.h>
#include <cpuid.h>
#include <accessClient.h>
#include <unistd.h>
#include <numa.h>
#include <msr.h>

int MAX_NUM_NODES = 4;

int is_supported()
{
    if (cpuid_init() == EXIT_FAILURE) {
        fprintf(stderr, "likwid-plugin.c: Unsupported processor!\n");
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

int hasDRAM(void)
{
    return ((cpuid_info.model == SANDYBRIDGE_EP) ||
            (cpuid_info.model == IVYBRIDGE_EP) ||
            (cpuid_info.model == HASWELL_EX) ||
            (cpuid_info.model == HASWELL));
}

int hasPP0(void)
{
    return ((cpuid_info.model == SANDYBRIDGE_EP) ||
            (cpuid_info.model == SANDYBRIDGE) ||
            (cpuid_info.model == IVYBRIDGE_EP) ||
            (cpuid_info.model == IVYBRIDGE) ||
            (cpuid_info.model == HASWELL) ||
            (cpuid_info.model == ATOM_SILVERMONT_E) ||
            (cpuid_info.model == ATOM_SILVERMONT_F1) ||
            (cpuid_info.model == ATOM_SILVERMONT_F2) ||
            (cpuid_info.model == ATOM_SILVERMONT_F3));
}

int hasPP1(void)
{
    return ((cpuid_info.model == HASWELL) ||
            (cpuid_info.model == SANDYBRIDGE) ||
            (cpuid_info.model == IVYBRIDGE));
}

void check_processor(Likwid_Plugin *likwid)
{
    likwid->hasDRAM = hasDRAM();
    likwid->hasPP0 = hasPP0();
    likwid->hasPP1 = hasPP1();
}

void get_power_info(Likwid_Plugin *likwid)
{
    int socket_fd = -1;
    accessClient_setaccessmode(1);
    accessClient_init(&socket_fd);
    msr_init(socket_fd);

    check_processor(likwid);
    numa_init();

    power_init(numa_info.nodes[0].processors[0]);

    strcpy(likwid->power_names[0], "likwid:::TDP");
    strcpy(likwid->power_names[1], "likwid:::Minimum Power");
    strcpy(likwid->power_names[2], "likwid:::Maximum Power");
    strcpy(likwid->power_names[3], "likwid:::Maximum Time Window");

    likwid->power_values[0] = power_info.tdp;
    likwid->power_values[1] = power_info.minPower;
    likwid->power_values[2] = power_info.maxPower;
    likwid->power_values[3] = power_info.maxTimeWindow;
}

void get_power_data(Likwid_Plugin *likwid, int duration_in_sec)
{
    int socket_fd = -1;
    check_processor(likwid);

    numa_init();

    int numSockets = numa_info.numberOfNodes;
    if (numSockets > cpuid_topology.numSockets) {
        printf("likwid::get_power_data() -- numsockets\n");
        numSockets = cpuid_topology.numSockets;
    }
    likwid->numSockets = numSockets;

    accessClient_setaccessmode(1);
    accessClient_init(&socket_fd);
    msr_init(socket_fd);

    int i;
    int cpuId[numSockets];
    for (i = 0; i < numSockets; i++) {
        cpuId[i] = numa_info.nodes[i].processors[0];
        power_init(cpuId[i]);
    }

    PowerData pDataPkg[MAX_NUM_NODES * 2];
    PowerData pDataDram[MAX_NUM_NODES * 2];
    PowerData pDataPP0[MAX_NUM_NODES * 2];
    PowerData pDataPP1[MAX_NUM_NODES * 2];

    for (i = 0; i < numSockets; ++i) {
        power_start(&(pDataPkg[i]), cpuId[i], PKG);
        if (likwid->hasDRAM) {
            power_start(&(pDataDram[i]), cpuId[i], DRAM);
        }
        if (likwid->hasPP0) {
            power_start(&(pDataPP0[i]), cpuId[i], PP0);
        }
        if (likwid->hasPP1) {
            power_start(&(pDataPP1[i]), cpuId[i], PP1);
        }

        sleep(duration_in_sec);

        power_stop(&(pDataPkg[i]), cpuId[i], PKG);
        if (likwid->hasDRAM) {
            power_stop(&(pDataDram[i]), cpuId[i], DRAM);
        }
        if (likwid->hasPP0) {
            power_stop(&(pDataPP0[i]), cpuId[i], PP0);
        }
        if (likwid->hasPP1) {
            power_stop(&(pDataPP1[i]), cpuId[i], PP1);
        }
    }

    double runtime = (double) duration_in_sec;

    for (i = 0; i < numSockets; ++i) {
        sprintf(likwid->sockets[i][0], "likwid:::Socket %d (PKG): Energy in Joules", i);
        sprintf(likwid->sockets[i][1], "%lf", power_printEnergy(&(pDataPkg[i])));
        sprintf(likwid->sockets[i][2], "likwid:::Socket %d (PKG): Power in Watts (%d secs)", i, duration_in_sec);
        sprintf(likwid->sockets[i][3], "%lf", power_printEnergy(&(pDataPkg[i])) / runtime);

        if (likwid->hasDRAM) {
            sprintf(likwid->dram[i][0], "likwid:::Socket %d (DRAM): Energy in Joules", i);
            sprintf(likwid->dram[i][1], "%lf", power_printEnergy(&(pDataDram[i])));
            sprintf(likwid->dram[i][2], "likwid:::Socket %d (DRAM): Power in Watts (%d secs)", i, duration_in_sec);
            sprintf(likwid->dram[i][3], "%lf", power_printEnergy(&(pDataDram[i])) / runtime);
        }

        if (likwid->hasPP0) {
            sprintf(likwid->PP0[i][0], "likwid:::Socket %d (PP0): Energy in Joules", i);
            sprintf(likwid->PP0[i][1], "%lf", power_printEnergy(&(pDataPP0[i])));
            sprintf(likwid->PP0[i][2], "likwid:::Socket %d (PP0): Power in Watts (%d secs)", i, duration_in_sec);
            sprintf(likwid->PP0[i][3], "%lf", power_printEnergy(&(pDataPP0[i])) / runtime);
        }

        if (likwid->hasPP1) {
            sprintf(likwid->PP0[i][0], "Socket %d (PP1): Energy in Joules", i);
            sprintf(likwid->PP0[i][1], "%lf", power_printEnergy(&(pDataPP1[i])));
            sprintf(likwid->PP0[i][2], "Socket %d (PP1): Power in Watts (%d secs)", i, duration_in_sec);
            sprintf(likwid->PP0[i][3], "%lf", power_printEnergy(&(pDataPP1[i])) / runtime);
        }
    }

    msr_finalize();
}
