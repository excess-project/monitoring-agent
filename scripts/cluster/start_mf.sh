#!/bin/bash
#  Copyright (C) 2016 University of Stuttgart
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

# start an mf agent on each of the computational nodes allocated in the pbs job
# !warning the script will be executed under root

DBKEY=$1
NODE=$2
PBS_JOBID=$3
PBS_USER=$4
PBS_JOBNAME=$5 # new argument introduced in 16.2

if [ -z "${PBS_JOBNAME}" ]; then
  PBS_JOBNAME="Unnamed_PBS_Job"
fi

LOG_FILE=/var/log/hpcmeasure/mf_service_prologue_${NODE}.log

HOME_USER=/nas_home/${PBS_USER}
MF_REVISION=$(cat /opt/mf/revision)
MF_TOP_PATH=/opt/mf/${MF_REVISION}
MF_BIN_PATH=${MF_TOP_PATH}/bin
MF_PLUGIN_PATH=${MF_BIN_PATH}/plugins
MF_SCRIPT_PATH=${MF_TOP_PATH}/scripts
MF_AGENT_PIDFILE_TOP_PATH=/ext/mf/plugins/${NODE}
MF_AGENT_PIDFILE=${MF_AGENT_PIDFILE_TOP_PATH}/${PBS_JOBID}
MF_USER_TOP_PATH=${HOME_USER}/.mf
DOMAIN=".fe.excess-project.eu"
JOBID=${PBS_JOBID%${DOMAIN}}
MF_AGENT_USER_CONFIGFILE=${MF_USER_TOP_PATH}/${JOBID}.ini
MF_AGENT_STD_CONFIGFILE=${MF_TOP_PATH}/mf_config.ini
#pre-check
DATE="$( date +'%c'  )"
echo $DATE":--start initializing part of start_mf.sh---" >> $LOG_FILE


if [ -z "${MF_AGENT_PIDFILE_TOP_PATH}" ]; then
  echo $DATE":Error in start_mf.sh: variable MF_AGENT_PIDFILE_TOP_PATH is not initialized" >> $LOG_FILE
  exit 0
fi
if [ -z "$MF_AGENT_PIDFILE" ]; then
  echo $DATE":Error in start_mf.sh: variable MF_AGENT_PIDFILE is not initialized" >> $LOG_FILE
  exit 0
fi
if [ ! -d "${MF_AGENT_PIDFILE_TOP_PATH}" ]; then
  mkdir ${MF_AGENT_PIDFILE_TOP_PATH} -p
fi
#log details about the job
echo $DATE":MF_AGENT_PIDFILE:"$MF_AGENT_PIDFILE >>$LOG_FILE
echo $DATE":DBKEY:"$DBKEY >>$LOG_FILE


#start mf_agent and save the process id
libs=/opt/mf/${MF_REVISION}/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libs

#check user config file
echo $DATE":Check for file ${MF_AGENT_USER_CONFIGFILE}" >> $LOG_FILE
if [ -e "${MF_AGENT_USER_CONFIGFILE}" ]; then
   echo $DATE":Using ${MF_AGENT_USER_CONFIGFILE} as configuration file" >> $LOG_FILE
   ${MF_BIN_PATH}/mf_agent -w ${PBS_USER} -t ${PBS_JOBNAME} -e ${DBKEY} -c ${MF_AGENT_USER_CONFIGFILE} &> /dev/null &
   MF_CONFIG_FILE=${MF_AGENT_USER_CONFIGFILE}
else
   echo	$DATE":Using ${MF_AGENT_STD_CONFIGFILE} as configuration file"	>> $LOG_FILE
   ${MF_BIN_PATH}/mf_agent -w ${PBS_USER} -t ${PBS_JOBNAME} -e ${DBKEY} -c ${MF_AGENT_STD_CONFIGFILE} &> /dev/null &
   MF_CONFIG_FILE=${MF_AGENT_STD_CONFIGFILE}
fi
MF_SERVICE_PID=$!
echo $MF_SERVICE_PID>${MF_AGENT_PIDFILE}

if [ ! -f "${MF_AGENT_PIDFILE}" ]; then
  echo $DATE":Error in start_mf.sh: file $MF_AGENT_PIDFILE is not written to disk. Abort." >> $LOG_FILE
  KILL_SIGNAL_OK="$( kill -SIGINT $MF_PLUGINS_PID )"
  if [[ "${KILL_SIGNAL_OK}" -ne "0" ]]; then
    echo $DATE":Error in start_mf.sh: KILL_SIGNAL for mf agent is not handled" >> $LOG_FILE
  else
    echo $DATE":MF agent with PID $MF_SERVICE_PID will be stopped." >> $LOG_FILE
  fi
fi

echo $DATE":---end ---" >> $LOG_FILE
