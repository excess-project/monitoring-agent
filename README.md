# EXCESS ATOM Monitoring Agent

> ATOM enables users to monitor applications at run-time with ease. In contrast to existing frameworks, our solution profiles applications with high resolution, focuses on energy measurements, and supports a heterogeneous infrastructure.


## Motivation
Reducing the energy consumption is a leading design constraint of current and future HPC systems. Aside from investing into energy-efficient hardware, optimizing applications is key to substantially reduce the energy consumption of HPC cluster. Software developers, however, are usually in the dark when it gets to energy consumption of their applications; HPC clusters rarely provide capabilities to monitor energy consumption on a fine granular level. Predicting the energy consumption of specific applications is even more difficult when the allocated hardware resources vary at each execution. In order to lower the hurdle of energy-aware development, we present ATOM---a light-weight neAr-real Time mOnitoring fraMework.


## Prerequisites

The monitoring agent requires first a running server and database. In order to install these requirements, please
checkout the associated [monitoring server][server], first. Please note that the installation and setup steps mentioned below assume that you are running a current Linux as operating system. We have tested the monitoring agent with Ubuntu 14.04 LTS as well as with Scientific Linux 6 (Carbon).

Before you can proceed, please clone the repository:

```bash
git clone git://github.com/excess-project/monitoring-agent.git
```


### Dependencies

This project requires the following dependencies to be installed:

| Component         | Homepage                                          | Version   |
|------------------ |-------------------------------------------------  |---------  |
| PAPI-C            | http://icl.cs.utk.edu/papi/                       | 5.4.0     |
| Apache APR        | https://apr.apache.org/                           | 1.5.1     |
| Apache APR Utils  | https://apr.apache.org/                           | 1.5.3     |
| Nvidia GDK        | https://developer.nvidia.com/gpu-deployment-kit   | 352.55    |

To ease the process of setting up a development environment, we provide a basic
script that downloads all dependencies, installs them locally in the project
directory, and then performs some clean-up operations. Thus, compiling the
monitoring agent can be performed in a sandbox without affecting your current
operating system.

Executing the following script

```bash
./setup.sh
```

results in a new directory named `bin`, which holds the required dependencies
for compiling the project.


## Installation

This section assumes that you've successfully installed all required dependencies as described in the previous paragraphs.

```bash
make
make install
```

The above commands compile and install the monitoring agent into the directory `dist` within the project's repository.
The `dist` folder includes all required binaries, shared libraries, scripts, and configuration files to get you started.


## Start monitoring

If you haven't yet followed our guide to set up the associated monitoring server and database, please do so now before
continuing. Please check once more, that a Elasticsearch database is running

```bash
curl localhost:9200
```

and that the monitoring server is running at

```bash
http://localhost:3000
```

Next, start the monitoring agent with a default set of plugins enabled to monitor, for instance, the memory consumption
of your current system as follows:

```bash
cd dist
./start.sh
```

You can learn more about various options passed to the monitoring agent by calling

```bash
./start.sh -h
```

While the monitoring agent collects metric data, you can already open the Web front-end located at

```bash
http://localhost:3000
```

You should see your first experiment being registered. The front-end allows to

- visualize sampled data
- download collected data as JSON
- download collected data as CSV


## Configuring plug-ins and update intervals

The monitoring agent as well as plug-ins are configurable at run-time by a global configuration file named `mf_config.ini`. The configuration is implemented by using an INI file; each section name such as `timings` or `plugins` is enclosed by square brackets. For each section, various parameters can be set. These parameters are custom-defined for each plug-in. For instance, the PAPI plug-in is activated by setting `mf\_plugin\_papi = on`, and it supports to profile, in this small example, four different counters, for which two are profiled: `PAPI\_FP\_INS` and `PAPI\_L1\_DCM`. The configuration file can be altered at run-time; new values are applied by default every 3 minutes (`update_configuration`).

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

Several parameters such as the `timing` of the plug-ins or the `hostname` where the server is running can be configured through this configuration file. The file is called `mf\_config.ini` and is located at `dist/mf\_config.ini`.


## Implementing new plugins

We provide more details on how to implement additional plugins [here][plugin-tutorial].


## Acknowledgment

This project is realized through [EXCESS][excess]. EXCESS is funded by the EU 7th
Framework Programme (FP7/2013-2016) under grant agreement number 611183. We are
also collaborating with the European project [DreamCloud][dreamcloud].


## Contributing
Find a bug? Have a feature request?
Please [create](https://github.com/excess-project/monitoring-agent/website/issues) an issue.


## Main Contributors

**Dennis Hoppe, HLRS**
+ [github/hopped](https://github.com/hopped)

**Fangli Pi, HLRS**
+ [github/hpcfapix](https://github.com/hpcfapix)

**Dmitry Khabi, HLRS**

**Yosandra Sandoval, HLRS**

**Anthony Sulisto, HLRS**


## Release History

| Date        | Version | Comment          |
| ----------- | ------- | ---------------- |
| 2016-02-26  | 16.2    | 2nd release (removed backend interface) |
| 2015-12-18  | 1.0     | Public release.  |


## License
Copyright (C) 2014-2016 University of Stuttgart

[Apache License v2](LICENSE).


[server]: https://github.com/excess-project/monitoring-server
[excess]: http://www.excess-project.eu
[dreamcloud]: http://www.dreamcloud-project.eu
[plugin-tutorial]: src/plugins/README.md
