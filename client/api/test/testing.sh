#!/bin/bash


WORKFLOW="ms2_v2"

## Register Workflow
curl -XPUT "http://localhost:9200/mf/workflows/ms2_v2" \
--data '{"application":"MS2","version":"v2","author":"Thomas Baumann","tasks":[{"name":"T1.task","type":"enter","next":"T2.task"},{"name":"T2.task","next":"T3.task"},{"name":"T3.task","type":"exit"}]}'

## Create a new experiment
CURRENT_TIME=$(date "+%Y.%m.%d-%H.%M.%S")
RESPONSE=$( curl -XPOST "http://localhost:9200/mf/experiments?parent=ms2_v2" \
--data '{"user":"hpcdhopp","description":"Profiling MS2","timestamp":"'"${CURRENT_TIME}"'"}' )

EXP_ID=$( echo $RESPONSE | awk -F'"' '{ print $12 }' )
echo "EXPERIMENT_ID:"$EXP_ID

## Create a new index for T1.task
curl -XPUT "http://localhost:9200/ms2_v2-t1.task" \
--data '{"dynamic_templates":[{"string_fields":{"mapping":{"index":"analyzed","omit_norms":true,"type":"string","fields":{"raw":{"index":"not_analyzed","ignore_above":256,"type":"string"}}},"match":"*","match_mapping_type":"string"}}],"_all":{"enabled":true},"properties":{"@timestamp":{"type":"date","format":"dateOptionalTime"},"host":{"type":"string","norms":{"enabled":false},"fields":{"raw":{"type":"string","index":"not_analyzed","ignore_above":256}}},"name":{"type":"string","norms":{"enabled":false},"fields":{"raw":{"type":"string","index":"not_analyzed","ignore_above":256}}},"value":{"type":"long","norms":{"enabled":false},"fields":{"raw":{"type":"long","index":"not_analyzed","ignore_above":256}}}}}'

## Some variables
DOMAIN=".fe.excess-project.eu"
MF_USER_TOP_PATH=${HOME}/.mf
WORKFLOWS_PATH=$MF_USER_TOP_PATH/workflows
mkdir -p $WORKFLOWS_PATH

## Create workflow files relevant to a job submission
##   $1: Job ID
##   $2: Experiment ID
##   $3: Name of workflow, e.g. ms2_v2
##   $4: Name of current task to be executed
function FUNC_CREATE_WORKFLOW_FILES {
    PBS_JOBID=${1%${DOMAIN}}
    BASENAME=${WORKFLOWS_PATH}/${PBS_JOBID}
    echo $2 > ${BASENAME}.dbkey
    echo $3 > ${BASENAME}.flow
    echo $4 > ${BASENAME}.task
}

## Execute T1.task
TASK="T1.task"
FUNC_CREATE_WORKFLOW_FILES $STEP1 $EXP_ID $WORKFLOW $TASK
#STEP1=$(qsub T1.task)
STEP1="1"
echo "Queued ${TASK}: "${STEP1}

STEP2_COMBINED=""

## Execute T2.1.task
TASK="T2.1.task"
FUNC_CREATE_WORKFLOW_FILES $STEP2 $EXP_ID $WORKFLOW $TASK
#STEP2=$(qsub -W depend=afterany:${STEP1} T2.1.task)
STEP2="2"
STEP2_COMBINED=${STEP2_COMBINED}:${STEP2}
echo "Queued ${TASK}: "${STEP2}

## Execute T2.2.task
TASK="T2.2.task"
FUNC_CREATE_WORKFLOW_FILES $STEP2 $EXP_ID $WORKFLOW $TASK
#STEP2=$(qsub -W depend=afterany:${STEP1} T2.2.task)
STEP2="3"
STEP2_COMBINED=${STEP2_COMBINED}:${STEP2}
echo "Queued ${TASK}: "${STEP2}

## Execute T2.3.task
TASK="T2.3.task"
FUNC_CREATE_WORKFLOW_FILES $STEP2 $EXP_ID $WORKFLOW $TASK
#STEP2=$(qsub -W depend=afterany:${STEP1} T2.2.task)
STEP2="4"
STEP2_COMBINED=${STEP2_COMBINED}:${STEP2}
echo "Queued ${TASK}: "${STEP2}

## Execute T2.4.task
TASK="T2.4.task"
FUNC_CREATE_WORKFLOW_FILES $STEP2 $EXP_ID $WORKFLOW $TASK
#STEP2=$(qsub -W depend=afterany:${STEP1} T2.2.task)
STEP2="5"
STEP2_COMBINED=${STEP2_COMBINED}:${STEP2}
echo "Queued ${TASK}: "${STEP2}

## Execute T2.5.task
TASK="T2.5.task"
FUNC_CREATE_WORKFLOW_FILES $STEP2 $EXP_ID $WORKFLOW $TASK
#STEP2=$(qsub -W depend=afterany:${STEP1} T2.2.task)
STEP2="6"
STEP2_COMBINED=${STEP2_COMBINED}:${STEP2}
echo "Queued ${TASK}: "${STEP2}

## Execute T3.task
TASK="T3.task"
FUNC_CREATE_WORKFLOW_FILES $STEP3 $EXP_ID $WORKFLOW $TASK
#STEP3=$(qsub -W depend=afterany:${STEP2_COMBINED} T3.task)
STEP3="7"
echo "Queued ${TASK}: "${STEP3}

#STEP2=${STEP2}:$(qsub -W depend=afterany:${STEP1} T2.2.task)
#STEP2=${STEP2}:$(qsub -W depend=afterany:${STEP1} T2.3.task)
#STEP2=${STEP2}:$(qsub -W depend=afterany:${STEP1} T2.4.task)
#STEP2=${STEP2}:$(qsub -W depend=afterany:${STEP1} T2.5.task)
#echo "Queued T2.1-T2.5: "${STEP2}

#STEP3=$(qsub -W depend=afterany:${STEP2} T3.task)
#echo "Queued T3: "${STEP3}
