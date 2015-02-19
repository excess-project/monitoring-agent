#!/bin/bash

## OUTPUT
OUTPUT_CSV="mf_overhead"
#RANDOM=cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 8 | head -n 1
OUTPUT_CSV="${OUTPUT_CSV}-${RANDOM}.csv"

DATA_BASE="${HOME}/cel_job/mv/data"
MESSAGES_DIR=${HOME}/cel_job/mv/messages/*.err

DOMAIN=".fe.excess-project.eu"

echo "plugins","frequency_in_ms","execution_time_in_ms" > $OUTPUT_CSV

for FILE in $MESSAGES_DIR; do
  echo "Parse:"$FILE

  ## Read JobID
  JOBID=$( basename "${FILE}" )
  JOBID=${JOBID%.*}
  JOBID=${JOBID%${DOMAIN}}

  ## Read database key
  DBKEY=$( grep -w 'id:' ${FILE}| awk -F"id:" '{print $2}'|head -n 1 )
  DBKEY=${DBKEY#[![:space:]]}

  ## Find performance file with execution times
  PERFORMANCE_FILE=$(grep -w "profile_filename" ${FILE}| awk -F"-profile_filename" '{print $2}'|head -n 1 )
  PERFORMANCE_FILEPATH=$(echo ${PERFORMANCE_FILE} | awk -F" -" '{print $1}')

  ## Extract configuration from performance filename
  PERFORMANCE_FILE="$(basename ${PERFORMANCE_FILEPATH})"
  CONFIGURATION=${PERFORMANCE_FILE}
  CONFIGURATION=${CONFIGURATION%%XXX*}
  PERFORMANCE_FILE=$( echo "${DATA_BASE}/$(basename "$PERFORMANCE_FILE")" )

  ## Get plugins used, and the set update frequency
  PLUGINS=()
  REMOVE_HEAD="MF_overhead_"
  REMOVE_PREFIX="mf_plugin_"
  TMP=$CONFIGURATION
  TMP=${TMP#${REMOVE_HEAD}}
  TMP=$(echo $TMP | sed "s/$REMOVE_PREFIX//g")
  IFS='-' read -a ARRAY <<< "$TMP"
  FREQUENCY=${ARRAY[${#ARRAY[@]}-1]}
  FREQUENCY=${FREQUENCY%%"ms"}

  unset ARRAY[${#ARRAY[@]}-1]
  PLUGINS=("${ARRAY[@]}")
  for PLUGIN in "${PLUGINS[@]}"; do
    COMBINATION="${COMBINATION}+${PLUGIN}"
  done
  COMBINATION=${COMBINATION#"+"}

  ## Get execution time from performance file
  CORES=0
  while IFS=$';' read -r -a DATA; do
    if [ ${DATA[0]} == "#id" ]; then
      continue
    fi
    CORES=${DATA[0]}

    ## Compute execution time based on nanoseconds
    SEC_START="${DATA[1]}"
    SEC_END="${DATA[3]}"
    SEC_DIFFERENCE=$(( $SEC_END - $SEC_START ))
    SEC_DIFFERENCE=$(( SEC_DIFFERENCE * 1000000000 ))
    NS_START="${DATA[2]}"
    NS_END="${DATA[4]}"
    NS_DIFFERENCE=$(( $NS_END - $NS_START ))

    ## Execution time in ms
    EXECUTION_TIME=$(( (SEC_DIFFERENCE + NS_DIFFERENCE) / 1000000 ))
    break
  done < $PERFORMANCE_FILE

  #echo $JOBID
  #echo $PERFORMANCE_FILE
  #echo $DBKEY
  #echo "Frequency:"$FREQUENCY
  #echo $COMBINATION
  #echo "Cores:"$CORES
  #echo "Execution time:"$EXECUTION_TIME

  echo "${COMBINATION}",${FREQUENCY},${EXECUTION_TIME} >> $OUTPUT_CSV
done
