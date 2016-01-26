#ifndef PERFMON_GROUP_TYPES_H
#define PERFMON_GROUP_TYPES_H


typedef enum {
    _NOGROUP = 0,

    BRANCH,

    CACHE,

    CLOCK,

    CPI,

    DATA,

    ENERGY,

    FLOPS_AVX,

    FLOPS_DP,

    FLOPS_SP,

    FLOPS_X87,

    FPU_EXCEPTION,

    ICACHE,

    L1TOL2,

    L2,

    L2CACHE,

    L2TOMEM,

    L3,

    L3CACHE,

    LINKS,

    MEM,

    MEM1,

    MEM2,

    MEM3,

    MEM4,

    MEM5,

    MEM6,

    MEM_DP,

    MEM_SP,

    NUMA,

    NUMA2,

    PAIRING,

    READ_MISS_RATIO,

    SCHEDULER,

    TLB,

    TLB_DATA,

    TLB_INSTR,

    VECTOR,

    VECTOR2,

    VIEW,

    VPU_FILL_RATIO_DBL,

    VPU_PAIRING,

    VPU_READ_MISS_RATIO,

    VPU_WRITE_MISS_RATIO,

    WRITE_MISS_RATIO,

    MAXNUMGROUPS
    } PerfmonGroup;

#endif
