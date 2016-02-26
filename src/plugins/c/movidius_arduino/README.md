# EXCESS ATOM Monitoring Framework: MOVIDIUS Plug-in

Movidius plugin read the arduino output from the serial interface (over an USB connector). The arduino is connected to the movidius MV0198 Daughtercard over the I2C interface. The Daughtercard has four MCP3424 chips, which measure the power comsumption of various movidius hardware components.

## Usage

The Movidius plug-in can be build and ran outside the EXCESS monitoring framework.
Please execute the **MAKEFILE** using

```
$ make
```

Before calling the sample client **mf_movidius_arduino_client**, please execute the
shell script **setenv.sh** to add required libraries to the **LD_LIBRARY_PATH**:

```
$ ./setenv.sh
```

It is advised to run the **mf_movidius_arduino_client** as follows:

```
$ ./mf_movidius_arduino_client <LIST_OF_METRICS>
```

Replace **<LIST_OF_METRICS>** with a space-separated list of valid Movidius events.
It follows a list of supported events in the next section.



## List of supported events

- VDDCR_A
- VDDCV_A
- DRAM_VDD1
- MIPI_VDD_A
- DRAM_VDD
- DRAM_VDDQ
- DRAM_MVDDQ
- DRAM_MVDDA
- USB_VDD330
- USB_VP_VDD
- VDDIO
- VDDIO_B
- RESERVED
- PLL_AVDD
- VDDCV_V
- MIPI_VDD_V


## Further information

For more information on Movidius, please have a look at the directory:

./arduino_source