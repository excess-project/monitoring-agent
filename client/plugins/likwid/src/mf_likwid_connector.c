#include <accessClient.h>
#include <cpuid.h>
#include <msr.h>
#include <numa.h>
#include <power.h>
#include <stdlib.h>
#include <unistd.h>

#include "mf_likwid_connector.h"

int MAX_NUM_NODES = 4;

static int
is_supported()
{
    if (cpuid_init() == EXIT_FAILURE) {
        fprintf(stderr, "mf_plugin_likwid.c: Unsupported processor!\n");
        return 0;
    }

    return 1;
}

char*
get_processor_model(void)
{
    is_supported();
    return cpuid_info.name;
}

int
is_processor_supported(void)
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
        fprintf(stderr, "Likwid not supported on this processor!\n");
        return 0;
    }

    return 1;
}

int
hasDRAM(void)
{
    return ((cpuid_info.model == SANDYBRIDGE_EP) ||
            (cpuid_info.model == IVYBRIDGE_EP) ||
            (cpuid_info.model == HASWELL_EX) ||
            (cpuid_info.model == HASWELL));
}

int
hasPP0(void)
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

int
hasPP1(void)
{
    return ((cpuid_info.model == HASWELL) ||
            (cpuid_info.model == SANDYBRIDGE) ||
            (cpuid_info.model == IVYBRIDGE));
}

void
check_processor(Likwid_Plugin *likwid)
{
    likwid->hasDRAM = hasDRAM();
    likwid->hasPP0 = hasPP0();
    likwid->hasPP1 = hasPP1();
}

void
get_power_info(Likwid_Plugin *likwid)
{
    int socket_fd = -1;
    accessClient_setaccessmode(1);
    accessClient_init(&socket_fd);
    msr_init(socket_fd);

    check_processor(likwid);
    numa_init();

    power_init(numa_info.nodes[0].processors[0]);

    strcpy(likwid->power_names[0], "likwid::TDP");
    strcpy(likwid->power_names[1], "likwid::min_power");
    strcpy(likwid->power_names[2], "likwid::max_power");
    strcpy(likwid->power_names[3], "likwid::max_time_window");

    likwid->power_values[0] = power_info.tdp;
    likwid->power_values[1] = power_info.minPower;
    likwid->power_values[2] = power_info.maxPower;
    likwid->power_values[3] = power_info.maxTimeWindow;
}

void
get_power_data(
    Likwid_Plugin *likwid,
    char **named_events,
    size_t num_events,
    int duration_in_sec /* not considered at the moment */)
{
    int socket_fd = -1;
    check_processor(likwid);

    numa_init();

    int numsockets = numa_info.numberOfNodes;
    if (numsockets > cpuid_topology.numsockets) {
        numsockets = cpuid_topology.numsockets;
    }
    likwid->numsockets = numsockets;

    accessClient_setaccessmode(1);
    accessClient_init(&socket_fd);
    msr_init(socket_fd);

    int i;
    int cpuId[numsockets];
    for (i = 0; i < numsockets; i++) {
        cpuId[i] = numa_info.nodes[i].processors[0];
        power_init(cpuId[i]);
    }

    PowerData pDataPkg[MAX_NUM_NODES * 2];
    PowerData pDataDram[MAX_NUM_NODES * 2];
    PowerData pDataPP0[MAX_NUM_NODES * 2];
    PowerData pDataPP1[MAX_NUM_NODES * 2];

    for (i = 0; i < numsockets; ++i) {
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

        //sleep(duration_in_sec);

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

    for (i = 0; i < numsockets; ++i) {
        sprintf(likwid->sockets[i][0], "likwid::socket%d::PKG::energy", i);
        sprintf(likwid->sockets[i][1], "%lf", power_printEnergy(&(pDataPkg[i])));
        //sprintf(likwid->sockets[i][2], "likwid::socket%d::PGK::power::%ds", i, duration_in_sec);
        //sprintf(likwid->sockets[i][3], "%lf", power_printEnergy(&(pDataPkg[i])) / runtime);

        if (likwid->hasDRAM) {
            sprintf(likwid->dram[i][0], "likwid::socket%d::DRAM::energy", i);
            sprintf(likwid->dram[i][1], "%lf", power_printEnergy(&(pDataDram[i])));
            //sprintf(likwid->dram[i][2], "likwid::socket%d::DRAM::power::%ds", i, duration_in_sec);
            //sprintf(likwid->dram[i][3], "%lf", power_printEnergy(&(pDataDram[i])) / runtime);
        }

        if (likwid->hasPP0) {
            sprintf(likwid->PP0[i][0], "likwid::socket%d::PP0::energy", i);
            sprintf(likwid->PP0[i][1], "%lf", power_printEnergy(&(pDataPP0[i])));
            //sprintf(likwid->PP0[i][2], "likwid:::socket%d::PP0::power::%ds", i, duration_in_sec);
            //sprintf(likwid->PP0[i][3], "%lf", power_printEnergy(&(pDataPP0[i])) / runtime);
        }

        if (likwid->hasPP1) {
            sprintf(likwid->PP1[i][0], "likwid::socket%d::PP1:energy", i);
            sprintf(likwid->PP1[i][1], "%lf", power_printEnergy(&(pDataPP1[i])));
            //sprintf(likwid->PP1[i][2], "likwid::socket%d::PP1::power::%ds", i, duration_in_sec);
            //sprintf(likwid->PP1[i][3], "%lf", power_printEnergy(&(pDataPP1[i])) / runtime);
        }
    }

    msr_finalize();
}
