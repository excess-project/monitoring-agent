# EXCESS ATOM Monitoring Framework: PAPI Plug-in

The PAPI plug-in is based on the PAPI-C library. A usage example is found in the
subfolder **src/utils**.



## Usage

The PAPI plug-in can be build and ran outside the EXCESS monitoring framework.
Please execute the **MAKEFILE** using

```
$ make
```

Before calling the sample client **mf_papi_client**, please execute the
shell script **setenv.sh** to add required libraries to the **LD_LIBRARY_PATH**:

```
$ ./setenv.sh
```

It is advised to run the **mf_papi_client** with root permissions, otherwise
no data can be collected via MSR registers:

```
$ ./mf_papi_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid PAPI events.
It follows a list of supported events in the next section.



## List of supported events

The list of supported events to be sampled depends on the underlying
infrastructure. A link to a full list of standard events is listed in the
further information section. In order to retrieve a list of available events
for your infrastructure, please execute the binary

```
$ ./papi_avail | grep Yes
```

which comes with the standard PAPI sources. If you have installed the
monitoring agent for development, and executed the **autobuild** shell script
located in the root folder, then the PAPI tool should be available at

```
$ <MONITORING_AGENT>/binaries/papi/bin
```



## Further information

For more information on PAPI, please have a look at the following sites:

- [PAPI](http://icl.cs.utk.edu/projects/papi)
- [PAPI Standard Events](http://icl.cs.utk.edu/projects/papi/presets.html)