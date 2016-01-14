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

#pre-check
DATE="$( date +'%c'  )"
echo $DATE":--start initializing part of stop_mf.sh---" >> $LOG_FILE

if [ ! -f "${MF_AGENT_PIDFILE}" ]; then
  echo $DATE":Error in stop_mf.sh: MF agent wasn't running" >> $LOG_FILE
fi

#log details about the job
echo $DATE":MF_AGENT_PIDFILE:"$MF_AGENT_PIDFILE >>$LOG_FILE
echo $DATE":DBKEY:"$DBKEY >>$LOG_FILE


#read the MF_AGENT_PIDFILE
MF_AGENT_PID="$( cat ${MF_AGENT_PIDFILE} )"
if [ ! -z "${MF_AGENT_PID}" ]; then
  #kill the mf agent
  KILL_SIGNAL_OK="$( kill -SIGINT  $MF_AGENT_PID )"
  if [[ "${KILL_SIGNAL_OK}" -ne "0" ]]; then
    echo $DATE":Error in stop_mf.sh: KILL_SIGNAL for mf agent is not handled" >> $LOG_FILE
  fi
  wait $MF_AGENT_PID &>> /dev/null
  echo $DATE":stop_mf.sh: The mf agent process is stopped" >> $LOG_FILE
  rm ${MF_AGENT_PIDFILE} -rf
else #MF_AGENT_PID
  echo $DATE":Error in stop_mf.sh: variable MF_AGENT_PID is not initialized" >> $LOG_FILE
fi

echo $DATE":---end ---" >> $LOG_FILE