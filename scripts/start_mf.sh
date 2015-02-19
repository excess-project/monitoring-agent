#!/bin/bash

#start an mf agent on each of the computational nodes allocated in the pbs job
#!warning the script will be executed under root

DBKEY=$1
NODE=$2
PBS_JOBID=$3
PBS_USER=$4
LOG_FILE=/var/log/hpcmeasure/mf_service_prologue_${NODE}.log

HOME_USER=/nas_home/${PBS_USER}
MF_REVISION=$(cat /opt/mf/revision)
MF_TOP_PATH=/opt/mf/${MF_REVISION}
MF_BIN_PATH=${MF_TOP_PATH}/bin
MF_SCRIPT_PATH=${MF_TOP_PATH}/scripts
MF_AGENT_PIDFILE_TOP_PATH=/ext/mf/plugins/${NODE}
MF_AGENT_PIDFILE=${MF_AGENT_PIDFILE_TOP_PATH}/${PBS_JOBID}
MF_USER_TOP_PATH=${HOME_USER}/.mf
MF_AGENT_USER_CONFIGFILE=${MF_USER_TOP_PATH}/${PBS_JOBID}.ini
MF_AGENT_STD_CONFIGFILE=${MF_TOP_PATH}/mf_config.ini
#pre-check
DATE="$( date +'%c'  )"
echo $DATE":--start initializing part of start_mf.sh---" >> $LOG_FILE


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
source ${MF_SCRIPT_PATH}/setenv.sh
#check user config file
if [ -f "${MF_AGENT_USER_CONFIGFILE}" ]; then
   ${MF_BIN_PATH}/mf_agent -id=${DBKEY} -config=${MF_AGENT_USER_CONFIGFILE} &
else
   ${MF_BIN_PATH}/mf_agent -id=${DBKEY} -config=${MF_AGENT_STD_CONFIGFILE} &
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

#log more details about the job
echo $DATE":---end ---" >> $LOG_FILE

exit 0
