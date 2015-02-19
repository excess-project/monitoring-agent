#!/bin/bash

# inputs
JOBS_DIR=$1
UPDATED_CONFIG=$2

# paths
MF_PATH=/opt/mf
REVISION=$(cat ${MF_PATH}/revision)
TOP_PATH=${MF_PATH}/${REVISION}
ORIGINAL_MF_CONFIG=${JOBS_DIR}/mf_config.ini

# backup mf_config.ini
cp -f ${ORIGINAL_MF_CONFIG} ${TOP_PATH}
