# README


## Motivation

The work package 3 (WP3) addresses the challenge of predicting the performance and energy consumption for applications during run-time. Prediction models for power consumption can then be utilized in order to achieve an optimal balance between performance and energy consumption. We address this challenge by introducing an energy-aware run-time system, which goes beyond state-of-the-art approaches by analyzing and optimizing not only the performance of applications, but also their energy consumption during run-time. This requires a thorough analysis of both the performance and energy consumption of applications and the systems' context in real-time.

When it comes to monitoring applications and infrastructure, state-of-the-art frameworks such as Icinga, Nagios, and Zabbix are well-known and proven solutions. In EXCESS, however, we require a monitoring framework to a) be scalable to a large amount of metrics to be collected for many parallel instances, b) introduce a low overhead to the actual application performance, c) allow for analysis and visualization during run-time, and d) be easy to extend. As evaluations conducted using the criteria defined by Telesca et al. reveal, Zabbix is the only viable candidate among eight frameworks coming into question to be installed for monitoring in EXCESS. This corresponds to the conclusions drawn by Telesca et al. for selecting Zabbix as the headline monitoring framework for the CERN Large Hadron Collider. In contrast to Telesca et al., our requirement of collecting and analyzing data at high frequencies has highest priority. Zabbix agents' update rate is per default at maximum only once per second, though, rendering Zabbix also as impractical to be used as the primary monitoring tool for EXCESS. We propose a new monitoring framework aiming at a fine granularity in metric resolution markedly above the update rate of once per second while collecting both performance and energy metrics in the same execution of an application. Since analyzing energy metrics are key to the success of EXCESS, we do not limit ourselves to metrics that rely on reading from processor registers; these registers maintain predictions for dissipated energy that, for instance, are based on recent hardware events such as Intel's Runtime Average Power Limits (RAPL). Instead, we successfully integrated a stable and reliable method for measuring the dissipated energy of hardware components at high frequencies using A/D converters. Besides power metrics, we support standard infrastructure-based metrics including the ones provided by PAPI-C. Metrics are sent at high frequencies to a distributed database powered by Elasticsearch using light-weight agents. Apart from collecting infrastructure data, user-customized and application-based metrics are easily integrable into the monitoring via a plug-in system, too. Finally, visualization of both historical and near real-time data is provided by utilizing the D3.js framework.


## Source Code

The source code of the monitoring framework is maintained in the Gitlab repository hosted at <pre>http://gitlab.excess-project.eu/<pre>. The framework can be checkout using git via the next command:

    git clone http://gitlab.excess-project.eu/hlrs/monitoring-framework-client.git

In order to execute the monitoring framework locally, you'll also need to checkout the corresponding server using:

    git clone http://gitlab.excess-project.eu/hlrs/monitoring-framework-server.git


## Local Installation (Linux-based operating system)

### Monitoring Framework Server (MFServer)

Firstly, you need to install a current version of <pre>NodeJS</pre> (>= 0.9), that at least supports the method <pre>setImmediate</pre>, which was introduced in version 0.9. Download the relevant binaries from [http://nodejs.org/download/](http://nodejs.org/download/), and add the binary folder to your environment path via:

    export PATH=<PATH_TO_NODE_BINARIES>:$PATH

Alternatively, you can use the software manager of your operating system to install <pre>NodeJS</pre>.

Secondly, install Elasticsearch by downloading and installing it following the steps listed at the [web-site](http://www.elasticsearch.org/overview/elkdownloads/). You can then start the Elasticsearch service via:

    sudo /etc/init.d/elasticsearch start

Finally, you can start the MFServer:

    node monitoring-framework-server/elasticnodetest1/app.js &

By doing so, you should now be able to access the Web-UI by visiting <pre>http://localhost:3000<pre>.


### Monitoring Framework Agent (MFAgent)

In order to build the MFAgent for the first time, please ensure that a current version of <pre>gfortran</pre> is installed; check:

    sudo apt-get install gfortran

Then, execute the <pre>autobuild.sh</pre> script in order to compile all dependencies:

    ./monitoring-framework-client/autobuild.sh

You can change the configuration of the MFAgent by altering <pre>mf_config.ini<pre>. Please ensure that the server address points to your local installation, e.g. <pre>http://localhost:3000/execution</pre>. Finally, you can start the MFAgent by executing the <pre>mf_agent</pre> binary:

    ./monitoring-framework-client/binaries/mf_agent