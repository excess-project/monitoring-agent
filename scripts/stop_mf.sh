#!/bin/bash

#end the mf agent each of the computational nodes allocated in the pbs job
#!warning the script will be executed under root

DBKEY=$1
NODE=$2
PBS_JOBID=$3
LOG_FILE=/var/log/hpcmeasure/mf_service_epilogue_${NODE}.log

MF_REVISION=$(cat /opt/mf/revision)
MF_TOP_PATH=/opt/mf/${MF_REVISION}
MF_BIN_PATH=${MF_REVISION}/bin
MF_SCRIPT_PATH=${MF_REVISION}/scripts
MF_AGENT_PIDFILE_TOP_PATH=/ext/mf/plugins/${NODE}
MF_AGENT_PIDFILE=${MF_AGENT_PIDFILE_TOP_PATH}/${PBS_JOBID}
MF_IOSTAT_PIDFILE=${MF_AGENT_PIDFILE_TOP_PATH}/${PBS_JOBID}.iostat

#pre-check
DATE="$( date +'%c'  )"
echo $DATE":--start initializing part of stop_mf.sh---" >> $LOG_FILE

if [ ! -f "${MF_AGENT_PIDFILE}" ]; then
  echo $DATE":Error in stop_mf.sh: MF agent wasn't running" >> $LOG_FILE
fi

#log details about the job
echo $DATE":MF_AGENT_PIDFILE:"$MF_AGENT_PIDFILE >>$LOG_FILE
echo $DATE":MF_IOSTAT_PIDFILE:"$MF_IOSTAT_PIDFILE >> $LOG_FILE
echo $DATE":DBKEY:"$DBKEY >>$LOG_FILE


#read the CONVERTER_PID
MF_AGENT_PID="$( cat ${MF_AGENT_PIDFILE} )"
if [ -z "${MF_AGENT_PID}" ]; then
  echo $DATE":Error in stop_mf.sh: variable MF_AGENT_PID is not initialized" >> $LOG_FILE
  exit 12
fi

#kill the mf agent
if [ -f "${MF_AGENT_PIDFILE}" ]; then
  KILL_SIGNAL_OK="$( kill -SIGINT  $MF_AGENT_PID )"
  if [[ "${KILL_SIGNAL_OK}" -ne "0" ]]; then
    echo $DATE":Error in stop_mf.sh: KILL_SIGNAL for mf agent is not handled" >> $LOG_FILE
  fi
  #echo $DATE":stop_mf.sh: The mf agent process will be stopped:"$KILL_SIGNAL_OK >> $LOG_FILE
  #wait
  wait $MF_AGENT_PID &>> /dev/null
  echo $DATE":stop_mf.sh: The mf agent process is stopped" >> $LOG_FILE
fi

#read the CONVERTER_PID
MF_IOSTAT_PID="$( cat ${MF_IOSTAT_PIDFILE} )"
if [ -z "${MF_IOSTAT_PID}" ]; then
  echo $DATE":Error in stop_mf.sh: variable MF_IOSTAT_PID is not initialized" >> $LOG_FILE
  exit 12
fi

#kill the iostat plugin
if [ -f "${MF_IOSTAT_PIDFILE}" ]; then
  KILL_SIGNAL=$( kill $MF_IOSTAT_PID )
  wait $MF_IOSTAT_PID &>> /dev/null
  if [[ "${KILL_SIGNAL}" -ne "0" ]]; then
    echo $DATE":Error in stop_mf.sh: KILL_SIGNAL for iostat plug-in is not handled" >> $LOG_FILE
  else
    echo $KILL_SIGNAL >> $LOG_FILE
  fi
  echo $DATE":stop_mf.sh: The iostat process is stopped (PID="$MF_IOSTAT_PID")" >> $LOG_FILE
fi

echo $DATE":---end ---" >> $LOG_FILE

rm ${MF_AGENT_PIDFILE} -rf
rm ${MF_IOSTAT_PIDFILE} -rf

exit 0
