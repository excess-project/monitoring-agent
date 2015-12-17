# EXCESS ATOM Monitoring Framework: RAPL Plug-in

The RAPL plug-in is based on the PAPI-C library. A usage example is found in the
subfolder **src/utils**.



## Usage

The RAPL plug-in can be build and ran outside the EXCESS monitoring framework.
Please execute the **MAKEFILE** using

```
$ make
```

Before calling the sample client **mf_rapl_client**, please execute the
shell script **setenv.sh** to add required libraries to the **LD_LIBRARY_PATH**:

```
$ ./setenv.sh
```

It is advised to run the **mf_rapl_client** with root permissions, otherwise
no data can be collected via MSR registers:

```
$ ./mf_rapl_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid RAPL events.
It follows a list of supported events in the next section.



## List of supported events

- PACKAGE_ENERGY:PACKAGE0
- PACKAGE_ENERGY:PACKAGE1
- DRAM_ENERGY:PACKAGE0
- DRAM_ENERGY:PACKAGE1
- PP0_ENERGY:PACKAGE0
- PP0_ENERGY:PACKAGE1
- THERMAL_SPEC:PACKAGE0
- THERMAL_SPEC:PACKAGE1
- MINIMUM_POWER:PACKAGE0
- MINIMUM_POWER:PACKAGE1
- MAXIMUM_POWER:PACKAGE0
- MAXIMUM_POWER:PACKAGE1
- MAXIMUM_TIME_WINDOW:PACKAGE0
- MAXIMUM_TIME_WINDOW:PACKAGE1



## Further information

For more information on RAPL, please have a look at the following sites:

- [Accessing RAPL through PAPI](http://icl.cs.utk.edu/projects/papi/wiki/PAPITopics:RAPL_Access)