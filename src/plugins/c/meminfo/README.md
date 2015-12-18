# EXCESS ATOM Monitoring Framework: /proc/meminfo Plug-in

The /proc/meminfo plug-in has a usage example located at **src/utils**.



## Usage

The /proc/meminfo plug-in can be build and ran outside the EXCESS monitoring
framework. Please execute the **MAKEFILE** using

```
$ make
```

Next, you can execute the demonstrator as follows:

```
$ ./mf_meminfo_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid PAPI events.
It follows a list of supported events in the next section.



- ## List of supported events

- MemTotal
- MemFree
- Buffers
- Cached
- SwapCached
- Active
- Inactive
- Active(anon)
- Inactive(anon)
- Active(file)
- Inactive(file)
- Unevictable
- Mlocked
- SwapTotal
- SwapFree
- Dirty
- Writeback
- AnonPages
- Mapped
- Shmem
- Slab
- SReclaimable
- SUnreclaim
- KernelStack
- PageTables
- NFS_Unstable
- Bounce
- WritebackTmp
- CommitLimit
- Committed_AS
- VmallocTotal
- VmallocUsed
- VmallocChunk
- HardwareCorrupted
- AnonHugePages
- HugePages_Total
- HugePages_Free
- HugePages_Rsvd
- HugePages_Surp
- Hugepagesize
- DirectMap4k
- DirectMap2M
- DirectMap1G