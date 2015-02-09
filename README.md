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




## Elastic Search

~~~bash
sudo apt-get install openjdk-6-jdk # if you haven't
sudo export JAVA_HOME=/usr/lib/jvm/java-6-openjdk-amd64 >> /etc/bash.bashrc
wget https://download.elasticsearch.org/elasticsearch/elasticsearch/elasticsearch-0.90.6.tar.gz
tar xvf elasticsearch-0.90.6.tar.gz
mv elasticsearch-0.90.6 elasticsearch
cd elasticsearch
sudo export ES_HOME=`(pwd)` >> /etc/bash.bashrc
cd ./bin
./elasticsearch -f  # to run in the foreground
~~~

Whenever you want, you can type the following, in order to see what the program do:

~~~
tail -f ES_HOME/logs/elasticsearch.log
~~~

Finally, you can kill the program with the pid. There are a lot of ways. A simple is the following:

~~~
ps -ef |grep elasticsearch
kill #PID result of previous command or more easy:

pkill -f elasticsearch
~~~