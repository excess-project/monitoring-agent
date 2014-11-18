# README


## Monitoring Framework Setup

### Server (Local Setup)

#### Node.js
- We need a NodeJS version that supports the setImmediate (>= 0.9)
- Download binaries or source from [http://nodejs.org/download/](http://nodejs.org/download/)
- Add binaries to your environment path via: export PATH=<PATH_TO_NODE_BINARIES>:$PATH
- e.g.: export PATH=/home/sane/Downloads/node/bin:$PATH

#### Elasticsearch
- Download package from [web-site](http://www.elasticsearch.org/overview/elkdownloads/)
- Start elasticsearch: sudo /etc/init.d/elasticsearch start

#### Starting the server
- node monitoring-framework-server/elasticnodetest1/app.js



### Client

- sudo apt-get install gfortran
- ./monitoring-framework-client/autobuild.sh
- export LD_LIBRARY_PATH=<PATH_TO_LIBS>/binaries/papi/lib
- e.g. export LD_LIBRARY_PATH=/home/sane/git/monitoring-framework-client/binaries/papi/lib

#### Client Configuration
- nano monitoring-framework-client/binaries/conf
- HPC host: http://141.58.0.8:3000/executions/
- Local host: http://localhost:3000/executions/

    # host address
    host: http://141.58.0.8:3000/executions/
    # timing for three plugins (one is the thread that handles the timings)
    timing_0: 1000000000ns
    timing_1: 1000000000ns
    timing_2: 1000000000ns
    # timing for sending the data
    timingSend: 0s
    # timing for checking the config file for changes
    timingCheck: 30s

#### Plugin configuration
- nano monitoring-framework-client/binaries/plugins/pluginConf

    #PAPI
    Events: PAPI_TOT_INS,PAPI_SP_OPS,PAPI_TOT_CYC

#### Client Usage
- ./monitoring-framework-client/binaries/mf_agent
