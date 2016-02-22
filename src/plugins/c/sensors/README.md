# EXCESS ATOM Monitoring Framework: sensors Plug-in

The sensors plug-in is based on the lm_sensors library to measure CPU's core's temperature. A usage example is found in the
subfolder **src/utils**.



## Usage

The sensors plug-in can be build and ran outside the EXCESS monitoring framework.
Please execute the **MAKEFILE** using

```
$ make
```

Before calling the sample client **mf_sensors_client**, please execute the
shell script **setenv.sh** to add required libraries to the **LD_LIBRARY_PATH**:

```
$ ./setenv.sh
```

It is advised to run the **mf_sensors_client** as follows:

```
$ ./mf_sensors_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid metrics.
It follows a list of supported events in the next section.



## List of supported metrics

- CPU0_Core 0
- CPU0_Core 1
- CPU1_Core 0
- CPU1_Core 1
...


## Further information

For more information on sensors, please have a look at the following sites:

- https://searchcode.com/codesearch/view/68001947/#l-290
- http://dev.man-online.org/man3/libsensors/