# Getting Started with the Monitoring Framework II


## EXCESS Cluster Setup

The monitoring framework is linked to the PBS resource manager, meaning that each time you start a job, the monitoring framework profiles the respective application.


### Enable/Disable Monitoring

In order to activate monitoring, make sure to create the following files:

    #!/bin/bash
    echo "Activating monitoring framework"
    mkdir -p $HOME/.mf/service/node02
    mkdir -p $HOME/.mf/service/node01
    touch $HOME/.mf/service/node02/start_monitoring
    touch $HOME/.mf/service/node01/start_monitoring


You can also deactivate monitoring by following the steps below:

    #!/bin/bash
    echo "Deactivating monitoring framework"
    rm -f $HOME/.mf/service/node02/start_monitoring
    rm -f $HOME/.mf/service/node01/start_monitoring


For the PBS job file, please ensure that a variable DBKEY is set prior to starting a job:

    echo "get unique key for the Excess monitoring framework"
    module load power/mf_shared/20
    DBKEY_STRING=$( get_unique_dbkey -d ${HOME}/.mf/dbkey/ -f ${PBS_JOBID} )
    DBKEY=$( echo ${DBKEY_STRING}| awk -v N=$N '{print $2}' )


### Enable/Disable Power Measurement

If you also want to integrate external power measurements, please create the following files in your HOME directory:

    #!/bin/bash
    echo "Activating power measurement on node02"
    mkdir -p $HOME/.pwm/node02
    touch $HOME/.pwm/node02/copy_raw_data
    touch $HOME/.pwm/node02/send_data_to_mf

Again, deactivating the power measurement is done via:

    #!/bin/bash
    echo "Deactivating power measurement on node02"
    rm -f $HOME/.pwm/node02/copy_raw_data
    rm -f $HOME/.pwm/node02/send_data_to_mf


### User-defined Configuration

In order to not override the default configuration file, you can place specific configuration into your HOME directory at

    $HOME/.mf

The filename should correspond to the current PBS Job ID, e.g. 60671.ini. If such a file exists, it will be used instead of the default configuration. You can retrieve the PBS Job ID as a return value of calling **qsub**.


## Web Interface

    http://mf.excess-project.eu