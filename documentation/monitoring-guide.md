# Getting Started with the Monitoring Framework

## How to install the monitoring framework onto your system

The monitoring framework has no specific software and hardware requirements. The framework is currently only tested and supported for Linux, and thus we cannot guarantee that the framework will also work with Windows.

Please read the general installation steps below in order to:

- install the required dependencies (i.e. Elasticsearch, Node.js, git),
- install the monitoring framework server (MFServer), and
- install at least one monitoring agent (MFAgent) onto the system to be monitored.


### Install dependencies

Requirements:

- Node.js (>= 0.9)
- Elasticsearch (>= 1.2.4)
- git


#### Elasticsearch (1.4.4)

    mkdir $HOME/tmp
    cd tmp
    wget https://download.elasticsearch.org/elasticsearch/elasticsearch/elasticsearch-1.4.4.tar.gz
    tar -xf elasticsearch-1.4.4.tar.gz
    sudo mv elasticsearch-1.4.4 /usr/local/elasticsearch


#### Node.js (0.12)

    wget http://nodejs.org/dist/v0.12.0/node-v0.12.0-linux-x64.tar.gz
    tar -xf node-v0.12.0-linux-x64.tar.gz
    sudo mv node-v0.12.0-linux-x64 /usr/local/nodejs


#### Install git

    sudo apt-get install git

or

    sudo yum install git



### Install MFServer

    git clone http://gitlab.excess-project.eu/hlrs/monitoring-framework-server.git
    sudo mv monitoring-framework-server /usr/local/mf-server


Please provide the following credentials when asked:

- Username: guest
- Password: anonymous


### Install MFAgent (v0.1.4)

    git clone http://gitlab.excess-project.eu/hlrs/monitoring-framework-client.git
    cd monitoring-framework-client
    git checkout release/0.1.4

Please provide the following credentials when asked:

- Username: guest
- Password: anonymous

#### Compilation

Edit **$HOME/tmp/monitoring-framework-client/client/Makefile** in order to reflect the following lines:

    REVISION = 0.1.4
    INSTALL_DIR = /usr/local/mf/$(REVISION)

Then proceed with building the MFAgent:

    cd $HOME/tmp/monitoring-framework-client
    apt-get install gfortran
    ./autobuild.sh
    cd client
    make
    sudo make install

The MFAgent is now installed at /usr/local/mf/0.1.4


## Configuration

The default configuration file is located at:

    /usr/local/mf/0.1.4/mf_config.ini

For more information on the specific parameters, see the next section on how to profile applications.


## How to profile an application?

Follow the following steps in order to start the MFServer:

    /usr/local/elasticsearch/bin/elasticsearch &
    export PATH=/usr/local/nodejs/bin:$PATH
    node /usr/local/mf-server/elasticnodetest1/app.js

Follow the following steps in order to start the MFAgent.

    nano /usr/local/mf/0.1.4/scripts/setenv.sh

Change the library path to the following:

    libs=/usr/local/mf/0.1.4/lib


Then, you can set the environment variables and start the MFAgent:

    source /usr/local/mf/0.1.4/scripts/setenv.sh
    /usr/local/mf/0.1.4/bin/mf_agent

Starting the MFAgent will then profile by default the memory usage of the system the MFAgent was started on. In order to profile, for example, some PAPI-C counters for specific CPU cores, follow the steps listed below:


### Profiling an application using PAPI-C

#### Example application (Prime Number Generator)

    mkdir $HOME/tmp
    cd $HOME/tmp
    wget http://dl.bintray.com/kimwalisch/primesieve/primesieve-5.4-linux-x64.tar.gz
    tar -xf primesieve-5.4-linux-x64.tar.gz
    sudo apt-get install libjpeg62


#### Configure the monitoring framework to profile PAPI-C counters

First, you have to enable the PAPI plugin of the monitoring framework by altering the configuration file (/usr/local/mf/0.1.4/mf_config.ini) as follows:

Enable the PAPI-C plugin:

    [plugins]
    mf_plugin_likwid  = off
    mf_plugin_papi    = off
    mf_plugin_meminfo = on
    mf_plugin_rapl    = off


Enable the following PAPI-C counters:

    [mf_plugin_papi]
    MAX_CPU_CORES   = 2
    PAPI_TOT_INS    = on

With PAPI-C enabled, you have to start the MFAgent with root permissions, because the plug-in needs global access to profile the system from a so called third-person perspective:

    sudo env LD_LIBRARY_PATH=/usr/local/mf/0.1.4/lib:$LD_LIBRARY_PATH ./mf_agent


Finally, start the application of interest

   $HOME/tmp/primesieve

and choose a very high upper bound (e.g. 1000000000000), set the thread size to 1, and set the sieve size of 2048 KBytes. The prime number generator should run for a while to compute prime numbers within the given range.

We will now pin the process to various CPU cores in order to validate that the PAPI-C counters are actually profile on a core level. For pinning, we need the following application:

    sudo apt-get install taskset

After getting the current PID of the **primesieves** process, we can pin the process to either CPU core 0 or 1 as follows:

    taskset -cp 0 <PID>

    taskset -cp 1 <PID>



## Accessing the Web interface

Per default, the Web interface is located at

    http://localhost:3000



## Known Issues

### Elasticsearch: Unable to revive connection

Please add the following line (keepAlive) to /usr/local/mf-server/elasticnodetest1/app.js

    var client = new elasticsearch.Client({
      host: 'localhost:9200',
      log: 'trace',
      keepAlive: false
    });
