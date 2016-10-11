# EXCESS ATOM Monitoring Framework: ACME Plug-in

The ACME plugin reads the output provided by the ACME cape, which measures power and temperature values of multiple channels for the BeagleBone Black (BBB) board. Since the ACME cape is accessible through ethernet (dhcp, hostname can be afterwards fixed) and a libiio deamon is installed, with the ACME plugin we simply read all the samples and calculate a average value for each specific channel available.

## Usage

The ACME plug-in can be build and ran outside the EXCESS monitoring framework.
Please execute the **MAKEFILE** using

```
$ make
```

Before calling the sample client **mf_acme_client**, please execute the
shell script **setenv.sh** to add required libraries to the **LD_LIBRARY_PATH**:

```
$ ./setenv.sh
```

It is advised to run the **mf_acme_client** as follows:

```
$ ./mf_acme_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid ACME channels and events.
It follows a list of supported events in the next section.



## List of supported events (in case of 4 probes)

- iio:device0:current
- iio:device0:vshunt
- iio:device0:vbus
- iio:device0:power
- iio:device1:current
- iio:device1:vshunt
- iio:device1:vbus
- iio:device1:power
- iio:device2:current
- iio:device2:vshunt
- iio:device2:vbus
- iio:device2:power
- iio:device3:current
- iio:device3:vshunt
- iio:device3:vbus
- iio:device3:power


## Further information

For more information on ACME cape, please have a look at the following links:

http://baylibre.com/acme/
http://wiki.baylibre.com/doku.php?id=acme:start
https://wiki.analog.com/resources/tools-software/linux-software/libiio
