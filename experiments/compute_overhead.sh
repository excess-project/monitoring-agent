#!/bin/bash

# Reference value for computing the overhead in execution time
REFERENCE_VALUE=$( echo 55255.0 | bc -l )
REFERENCE_VALUE=$( echo "scale=2; $REFERENCE_VALUE / 100.0" | bc -l )

DATA_BASE=$1
MESSAGES_DIR=${DATA_BASE}/messages/*.err

## OUTPUT
OUTPUT_CSV="mf_overhead"
OUTPUT_CSV="${1}/${OUTPUT_CSV}-${RANDOM}.csv"

DOMAIN=".fe.excess-project.eu"

echo "plugins","frequency_in_ms","execution_time_in_ms","overhead" > $OUTPUT_CSV

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
  PERFORMANCE_FILE=$( echo "${DATA_BASE}/data/$(basename "$PERFORMANCE_FILE")" )

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
  COMBINATION=""
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
    SEC_DIFFERENCE=$( echo "scale=2; $SEC_END - $SEC_START" | bc -l )
    SEC_DIFFERENCE=$( echo "scale=2; $SEC_DIFFERENCE * 1000000000.0" | bc )
    NS_START="${DATA[2]}"
    NS_END="${DATA[4]}"
    NS_DIFFERENCE=$( echo "scale=2; $NS_END - $NS_START" | bc -l )

    ## Execution time in ms
    SUM=$( echo "scale=2; $SEC_DIFFERENCE + $NS_DIFFERENCE" | bc -l);
    EXECUTION_TIME=$( echo "scale=2; $SUM / 1000000.0" | bc -l )
    break
  done < $PERFORMANCE_FILE

  #echo $JOBID
  #echo $PERFORMANCE_FILE
  #echo $DBKEY
  #echo "Frequency:"$FREQUENCY
  #echo $COMBINATION
  #echo "Cores:"$CORES
  #echo "Execution time:"$EXECUTION_TIME
  OVERHEAD=$( echo "scale=2; $EXECUTION_TIME / $REFERENCE_VALUE" | bc -l )
  #CMP=$( echo "scale=2; $EXECUTION_TIME >= 100.0" | bc )
  if [ "$( echo "scale=2; $EXECUTION_TIME >= 100.0" | bc)" -eq 1 ]; then
    OVERHEAD=$( echo "scale=2; $OVERHEAD - 100.0" | bc -l)
  else
    OVERHEAD=$( echo "scale=2; 100.0 - $OVERHEAD" | bc -l)
  fi

  echo $OVERHEAD
  echo "${COMBINATION}",${FREQUENCY},${EXECUTION_TIME},${OVERHEAD} >> $OUTPUT_CSV
done