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
MF_PLUGIN_PATH=${MF_BIN_PATH}/plugins
MF_SCRIPT_PATH=${MF_TOP_PATH}/scripts
MF_AGENT_PIDFILE_TOP_PATH=/ext/mf/plugins/${NODE}
MF_AGENT_PIDFILE=${MF_AGENT_PIDFILE_TOP_PATH}/${PBS_JOBID}
MF_IOSTAT_PIDFILE=${MF_AGENT_PIDFILE_TOP_PATH}/${PBS_JOBID}.iostat
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
source ${MF_SCRIPT_PATH}/setenv.sh
#check user config file
echo $DATE":Check for file ${MF_AGENT_USER_CONFIGFILE}" >> $LOG_FILE
if [ -e "${MF_AGENT_USER_CONFIGFILE}" ]; then
   echo $DATE":Using ${MF_AGENT_USER_CONFIGFILE} as configuration file" >> $LOG_FILE
   ${MF_BIN_PATH}/mf_agent -id=${DBKEY} -config=${MF_AGENT_USER_CONFIGFILE} &
   MF_CONFIG_FILE=${MF_AGENT_USER_CONFIGFILE}
else
   echo	$DATE":Using ${MF_AGENT_STD_CONFIGFILE} as configuration file"	>> $LOG_FILE
   ${MF_BIN_PATH}/mf_agent -id=${DBKEY} -config=${MF_AGENT_STD_CONFIGFILE} &
   MF_CONFIG_FILE=${MF_AGENT_STD_CONFIGFILE}
fi
MF_SERVICE_PID=$!
echo $MF_SERVICE_PID>${MF_AGENT_PIDFILE}

# start iostat
SERVER=$( cat ${MF_CONFIG_FILE} | grep server | awk -F" " '{ print $3 }' )
CPU=$( cat ${MF_CONFIG_FILE} | grep IOSTAT_CPU | awk -F" " '{ print $3 }' )
DISK=$( cat ${MF_CONFIG_FILE} | grep IOSTAT_DISK | awk -F" " '{ print $3 }' )
if [[ $CPU = "on" && $DISK = "on" ]]; then
  iostat -k 1 | awk -v server=${SERVER} -v id=${DBKEY} -f ${MF_PLUGIN_PATH}/iostat.awk &
  echo $DATE":iostat plug-in "${MF_PLUGIN_PATH}/" started for server "$SERVER" with "$DBKEY >> $LOG_FILE
elif [[ $CPU == "on" ]]; then
  iostat -c -k 1 | awk -v server=${SERVER} -v id=${DBKEY} -f ${MF_PLUGIN_PATH}/iostat.awk &
  echo $DATE":iostat plug-in "${MF_PLUGIN_PATH}/" started for server "$SERVER" with "$DBKEY >> $LOG_FILE
elif [[ $DISK == "on" ]]; then
  iostat -d -k 1 | awk -v server=${SERVER} -v id=${DBKEY} -f ${MF_PLUGIN_PATH}/iostat.awk &
  echo $DATE":iostat plug-in "${MF_PLUGIN_PATH}/" started for server "$SERVER" with "$DBKEY >> $LOG_FILE
fi

MF_IOSTAT_PID=$!
echo $MF_IOSTAT_PID>${MF_IOSTAT_PIDFILE}
echo $DATE":MF_IOSTAT_PIDFILE:"$MF_IOSTAT_PIDFILE >> $LOG_FILE

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
