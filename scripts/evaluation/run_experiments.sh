#!/bin/bash

## Power
ACTIVATE_HWPOWER=1
ACTIVATE_MF=1

## Jobs
#TOP_PATH=/nas_home/hpcdhopp/cel_job/mv
TOP_PATH=.
TIMESTAMP=$(date +"%Y-%m-%d_%H-%M-%S")
JOBS_DIR="${TOP_PATH}/jobs-${TIMESTAMP}"
mkdir -p $JOBS_DIR
JOBS_DONE="${JOBS_DIR}/jobs_submitted"
mkdir -p $JOBS_DIR/scripts
mkdir -p $JOBS_DIR/configurations

## Create configurations first
CONFIGURATION_SCRIPT=./create_configurations.sh
$CONFIGURATION_SCRIPT
CONFIGURATIONS_DIR=configurations/*

## mf_config.ini location
REVISION=$(cat $HOME/mf_test/revision)
MF_CONFIG_FILE=$HOME/mf_test/$REVISION/mf_config.ini

## Benchmark
BENCHMARK_TEMPLATE=run_jobs_on_hornet.template
BENCHMARK_SH="run_jobs_on_hornet_overhead.sh"


## Activate/Deactivate power measurements on EXCESS cluster
HW_POWER="hwpower_"
./activate_hwpower.sh ${ACTIVATE_HWPOWER}
if [ $ACTIVATE_HWPOWER -eq 0 ]; then
  HW_POWER=""
fi

MONITORING="mf_"
./activate_mf.sh ${ACTIVATE_MF}
if [ $ACTIVATE_MF -eq 0 ]; then
  MONITORING=""
fi


## Create a PBS job for each configuration
REMOVE_PREFIX="mf_config-"
for FILE in $CONFIGURATIONS_DIR; do
  if [ -e "$FILE" ]; then
    ## Execute benchmark with current configuration
    cp $BENCHMARK_TEMPLATE ${TOP_PATH}/$BENCHMARK_SH

    NEW_PREFIX=$(basename $FILE)
    NEW_PREFIX="MF_overhead_${HW_POWER}${MONITORING}${NEW_PREFIX#${REMOVE_PREFIX}}"
    NEW_PREFIX="${NEW_PREFIX%.*}XXX_"

    SED_STRING="s/PREFIX=/PREFIX=\"$NEW_PREFIX\"/g"
    sed -i "${SED_STRING}" ${BENCHMARK_SH}

    SED_STRING="s/USER_JOBS_DIR/${JOBS_DIR}/g"
    sed -i "${SED_STRING}" ${BENCHMARK_SH}

    SED_STRING="/USER_CONFIG_FILE/${FILE}/g"
    sed -i "${SED_STRING}" ${BENCHMARK_SH}

    echo  "Initialize job submission for ${NEW_PREFIX}"
    cd ${TOP_PATH} && ./$BENCHMARK_SH

    cp ${TOP_PATH}/$BENCHMARK_SH "${JOBS_DIR}/scripts/${NEW_PREFIX}.sh"
    TIMESTAMP=$(date +"%Y-%m-%d_%H-%M-%S")
    echo $TIMESTAMP,$USER,$(basename $FILE) >> $JOBS_DONE
  fi
done