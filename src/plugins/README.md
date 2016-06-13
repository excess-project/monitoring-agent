# Supported Plug-ins

- [Infiniband](c/infiniband/README.md)
- [/proc/meminfo](c/meminfo/README.md)
- [/proc/vmstat](c/vmstat/README.md)
- [Nvidia GPUs](c/nvidia/README.md)
- [PAPI-C](c/papi/README.md)
- [RAPL](c/rapl/README.md)


# Plug-in Development Tutorial

## Summary

This tutorial covers the basics to enable developers to write custom C-based plug-ins to be integrated into the EXCESS monitoring framework. The tutorial is divided into three parts: Firstly, the data format for sending metric data to the database is detailed. Secondly, a C skeleton is presented, that can be used as the basis to write new plug-ins is presented. Finally, the compiling, deploying, and configuring process of new plug-in is illustrated.


## Data Format

This section details the data exchange format used for the communication between the key components, namely the server and the monitoring agents. A metric is represented by its name and a series of numerical values collected over time. We can then represent a specific data point in time by a simple key-value pair composed of the metric name and its numeric value; the pair is further associated with a specific time when the data point was measured, and the host name with which the measurement took place. The next listing illustrates the resulting JSON document that is based on the above verbal description of a metric; a metric is represented by a JSON object.

<pre>
{
  "Timestamp": 1418041226.201397,
  "hostname": "fe.excess-project.eu",
  "type": "performance",
  "CP0:PAPI_L2_DCA": 4172129,
}
</pre>

The minimal document is then extended two-fold. Firstly, an additional parameter named *type* describes the type of metric data collected. When profiling performance data, one can assign, for example, the value *performance*. The *type* parameter is intended to be used to categorize metric data, e.g. by using the name of the plug-in or the type of metric measured; categorization is in general beneficial for data visualization at a later stage. In the future, it is planned that the *type* parameter will help to identify a group of metrics of interest in order to facilitate analysis and visualization of correlated data. Secondly, we extend the meaning of a JSON object from describing a single metric to aggregating all metrics collected by a given EXCESS plug-in. Hence, a JSON object encapsulates all information associated with a plug-in. It can be extended by an arbitrary number of key-value pairs, where each pair describes another metric measured by the plug-in. The next listing finally shows the data exchange document used by the EXCESS monitoring framework and its components.

```bash
[
  {
    "Timestamp": 1418041226.201397,
    "hostname": "fe.excess-project.eu",
    "type": "perfomance",
    "CPU0::PAPI_L2_DCA": 4172129,
    "CPU0::PAPI_TOT_INS": 156559634,
    "CPU1::PAPI_L2_DCA": 3956110,
    "CPU1::PAPI_TOT_INS": 144450317,
    "CPU2::PAPI_L2_DCA": 167164,
    "CPU2::PAPI_TOT_INS": 3959584,
    "CPU3::PAPI_L2_DCA": 45513,
    "CPU3::PAPI_TOT_INS": 499789
  },
  {
    "Timestamp": 1418041227.2014813,
    "hostname": "fe.excess-project.eu",
    "type": "energy",
    "PACKAGE_ENERGY:PACKAGE0": 12.9468,
    "PACKAGE_ENERGY:PACKAGE1": 13.5495,
    "DRAM_ENERGY:PACKAGE0": 3.89,
    "DRAM_ENERGY:PACKAGE1": 3.8404,
    "PP0_ENERGY:PACKAGE0": 3.1814,
    "PP0_ENERGY:PACKAGE1": 3.4632
  }
]
```

Since the principal components of a metric's data point are the value measured and the time of measurement, a metric is represented by the C struct *metric\_t* as shown next.

```c
#include <time.h>

typedef struct metric_t* metric;

struct metric_t {
    struct timespec timestamp;
    char *message;
} metric_t;
```

The C struct is composed of a *timestamp* and a *message* part. The *timestamp* is defined via a *timespec* struct; nanosecond resolution is used throughout the framework to ensure high accuracy of data aggregated at run-time. Furthermore, the pointer to characters named *message* is used to store the actual metric data. Metric data is again represented by key-value pairs using JSON formatting; the *type* parameter can be set, too. Next, an example is shown to instantiate and set the values of a *metric\_t* struct. It should be noted that information such as the *timestamp*, *hostname*, and *username* will be automatically added to the JSON object by the framework.

```c
/* example data describing the memory usage */
double mem_used = 20;
double mem_available = 100.0 - usage;

/* initialize metric_t */
metric mem_metric = malloc(sizeof(metric_t));
mem_metric->message = malloc(254 * sizeof(char));

/* set the timestamp */
int clk_id = CLOCK_REALTIME;
clock_gettime(clk_id, &mem_metric->timestamp);

/* copy JSON object to mem_metric->message */
sprintf(mem_metric->message,
  ",\"type\":\"memory\",\"mem_used\":%.2f,\"mem_avail\":%.2f",
  mem_used,
  mem_available
);
```


## C Skeleton for Plug-Ins

When adding new plug-ins for the prototype, the plug-in has to be written in C and compiled as a shared object. The next listing shows the fundamental structure of a plug-in that developers have to follow.

```c
#include <stdlib.h>
#include <string.h>

#include "plugin_manager.h"
#include "excess_main.h"
#include "util.h"

static metric
custom_plugin_hook()
{
  if (running) { /* running is defined in util.h */
    metric custom_metric = malloc(sizeof(metric_t));
    /* setup the metric as shown in the previous listing */
    return custom_metric;
  } else {
    return NULL;
  }
}

extern int
init_custom_plugin(PluginManager *pm)
{
  PluginManager_register_hook(pm, "custom_plugin", custom__plugin_hook);
  return 1;
}
```

When creating a plug-in one needs to comply with some rules in order to make the plug-in work with the MFAgents. The skeleton consists of two main functions: a function hook (e.g. *custom\_plugin\_hook*), and a function for initialization (e.g. *init\_custom\_plugin*). The name of the function hook (cf. line 9) has to match the name defined while registering the hook (cf. line 23).

It should be noted that the name of the shared object file needs to match the name given to the plugin (e.g. *custom*), otherwise the internal functions cannot be loaded automatically by the MFAgent's plug-in manager. The function *init\_custom\_plugin* is, following the example above, the entry point of the plug-in. It is used to register the function *custom\_plugin\_hook* to the plug-in management routines. *custom\_plugin\_hook* is then used to gather metric data, and returns a pointer to a C struct, which contains the values of the metric as well as the timestamp of measurement. Please note that plug-ins are intended to gather metrics and it is not recommended to use them for other purposes as it can result in undefined behavior. The MFAgents will take care of the update rate when new metric data has to be collected. As a consequence, the plug-in itself does not need to implement such kind of functionality. Please note, that the function  *custom\_plugin\_hook* is called each time new data has to be collected.

The next section details, among other things, how developers can configure the plug-in using the global INI configuration file.


## Compilation, Deployment and Configuration


### Compilation

Please refer to the main guide [here](../../README).


### Deployment

In order to use a custom plug-in at run-time, the compiled shared object file needs to be put into the plug-ins folder of the monitoring framework on the EXCESS cluster---if not automatically copied through the Makefile. The path to the plug-ins folder is as follows, where the *<version>*-tag has to be replaced with the current version running on the cluster: */opt/mf/<version>/bin/plugins*.


### Configuration (mf_config.ini)

MFAgents as well as plug-ins are configurable at run-time by a global configuration file (shown below for the PAPI plug-in). The configuration is implemented by using an INI file; each section name such as *job* or *plugins* is enclosed by square brackets. For each section, various parameters can be set. These parameters are custom-defined for each plug-in. For instance, the PAPI plug-in is activated (*mf\_plugin\_papi = on*; line 9), and it supports to profile five different counters, for which two are profiled: *PAPI\_DP\_OPS* and *PAPI\_FP\_INS* (cf. lines 18 to 22). The configuration file can be altered at run-time; new values are applied by default every 30 seconds.

```bash
;EXCESS ATOM Monitoring Framework Configuration

[generic]
server = http://localhost:3000/executions/

[plugins]
mf_plugin_papi    = on
mf_plugin_meminfo = off

[timings]
default               = 100000000ns
publish_data_interval = 0s
update_configuration  = 360s
mf_plugin_papi        = 1000000000ns
mf_plugin_meminfo     = 1000000000ns

[mf_plugin_papi]
MAX_CPU_CORES = 8
PAPI_FP_INS  = on
PAPI_LST_INS = off
PAPI_L1_DCM  = on
PAPI_FLOPS   = off
```

Several parameters such as the timing of the plug-ins or the hostname where the server is running can be configured through this configuration file. The file is called *mf\_config.ini* and is located at */opt/mf/<version>*. When adding a new plug-in, the following changes to the configuration file have to be performed:

- add the new plugin to the *plugin* section (required)
- add a new section for the plug-in for additional settings (optional)

Since plug-ins can be activated or deactivated prior to profiling, the section on plug-ins has to be extended by the new plug-in as shown below for the *mem\_info} plug-in.

```bash
[plugins]
mf_plugin_papi    = on
mf_plugin_meminfo = off
```

Aside from adding the plug-in itself, developers can also define some custom parameters that can be set for the plug-in by creating a new section within the configuration file dedicated to the new plugin. For example, the section *mem\_info* holds additional information stating if some specific metric data should actually be profiled at run-time as detailed next.

```bash
[mf_plugin_papi]
MAX_CPU_CORES = 8
PAPI_FP_INS  = on
PAPI_LST_INS = off
PAPI_L1_DCM  = on
PAPI_FLOPS   = off
```

It should be noted, that these parameters have to be read out at run-time within the plug-in using the parser component offered by the monitoring framework.


#### Configuration Parser

The monitoring framework is shipped with a parser component named *mf\_parser*. The parser reads the configuration file, where each section is accessible independently. The parameters for each section are stored internally as character arrays. The general API is presented next.

```c
/**
 * @brief Parses a given file.
 * @return 1 if successful; 0 otherwise.
 */
int mfp_parse(const char* filename);

/**
 * @brief Returns a stored value for the given section and key.
 * @return the value stored for the index <section, key>
 */
char* mfp_get_value(const char* section, const char* key, char *ret_val);

/**
 * @brief Sets or overwrites the value for a given section and key.
 */
void mfp_set_value(const char* section, const char* key,const char* value);

/**
 * @brief Returns the entire data stored for a given section
 */
void mfp_get_data(const char* section, mfp_data* data);

/**
 * @brief Filters the data based on the given filter.
 *
 * Example: Setting the filter to "on" will retain those keys in the data
 * set which value equals the filter value.
 */
void mfp_get_data_filtered_by_value(
  const char* section,
  mfp_data* data,
  const char* filter_by_value
);
```

The next listing shows the usage of the API methods and functions with respect to the PAPI plug-in; the struct *mfp\_data* can be seen in action (cf. line~9), as well as reading parameters using the *mfp\_get\_data\_filtered\_by\_value* method (cf. line~22). The latter method stores both keys and values into the variable *conf\_data*.

```c
static metric
papi_plugin_hook()
{
  if (running) {
    metric papi_metric = malloc(sizeof(metric_t));
    papi_metric->message = malloc(1024 * sizeof(char));

    /* allocate memory for the parser struct */
    mfp_data *conf_data = malloc(sizeof(mfp_data));

    int clk_id = CLOCK_REALTIME;
    clock_gettime(clk_id, &papi_metric->timestamp);
    int sleep_in_ms = 500000; // 0.5s

    /* read out active counters for PAPI */
    mfp_get_data_filtered_by_value("papi", conf_data, "on");

    mf_papi_init(conf_data->keys, conf_data->size);
    mf_papi_profile(sleep_in_ms);

    PAPI_Plugin *papi = malloc(sizeof(PAPI_Plugin));
    mf_papi_read(papi, conf_data->keys);

    strcpy(papi_metric->message, to_JSON(papi));

    free(papi);
    free(conf_data);

    return papi_metric;
  } else {
    return NULL;
  }
}
```
