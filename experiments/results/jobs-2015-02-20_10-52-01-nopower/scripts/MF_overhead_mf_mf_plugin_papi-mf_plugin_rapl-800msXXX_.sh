#!/bin/sh

USER_LUSTRE_DIR_SED="\/nas_home\/hpcdhopp\/cel_job\/mv\/data"
USER_PROJ_DIR_SED="\/nas_home\/hpcdhopp\/cel_job\/mv"
USER_BIN_SED="\/nas_home\/hpcdhopp\/cel_job\/mv\/bin\/cel_benchmark"
USER_BIN_PAT_SED=

USER_RUN_ATTEMPTS="\"50000\""
ITER_MAX=100
USER_TOL_EP="\"1.0E-16\""
USER_RUN_CHECK_RESULT="\"0\""
USER_VERBOSITY=0
USER_NUM_MASTERS_PER_NODE=2
USER_NUM_MASTERS_PER_2NODES=2
USER_RUN_APRUN_PARM_1="\" -f ${USER_PROJ_DIR_SED}\/nodefile \""
USER_RUN_APRUN_PARM_2="\" -f ${USER_PROJ_DIR_SED}\/nodefile \""
USER_RUN_APRUN_PARM_N="\" -f ${USER_PROJ_DIR_SED}\/nodefile \""



USER_PROCS_ARRAY="( \"0\" \"1\"   \"2\"   \"4\"   \"8\"    \"12\"   \"16\"   \"32\"    \"48\"   \"64\"  \"96\"  \"128\"  \"192\"  \"256\"  \"384\"  \"512\"  \"768\"   \"1024\"  \"1536\"  \"2048\"  )"

USER_XX="(\"0\"           \"32\"  \"64\" \"64\" \"64\" \"96\"  \"128\" \"128\" \"192\" \"128\" \"192\" \"256\"  \"384\"  \"256\"  \"384\"  \"256\"  \"384\"   \"512\"   \"768\"   \"512\"   )"
USER_YY="(\"0\"           \"32\"  \"32\" \"64\" \"64\" \"64\"  \"64\"  \"128\" \"128\" \"128\" \"128\" \"128\"  \"128\"  \"256\"  \"256\"  \"256\"  \"256\"   \"256\"   \"256\"   \"512\"   )"
USER_ZZ="(\"0\"           \"32\"  \"32\" \"32\" \"64\" \"64\"  \"64\"  \"64\"  \"64\"  \"128\" \"128\" \"128\"  \"128\"  \"128\"  \"128\"  \"256\"  \"256\"   \"256\"   \"256\"   \"256\"   )"

USER_DX="( \"0\"          \"1\"   \"2\"  \"2\"   \"2\"  \"3\"  \"4\"  \"4\"   \"6\"   \"4\"   \"6\"    \"8\"   \"12\"   \"8\"    \"12\"   \"8\"    \"12\"    \"16\"     \"24\"   \"16\"    )"
USER_DY="( \"0\"          \"1\"   \"1\"  \"2\"   \"2\"  \"2\"  \"2\"  \"4\"   \"4\"   \"4\"   \"4\"    \"4\"   \"4\"    \"8\"    \"8\"    \"8\"    \"8\"     \"8\"      \"8\"    \"16\"    )"
USER_DZ="( \"0\"          \"1\"   \"1\"  \"1\"   \"2\"  \"2\"  \"2\"  \"2\"   \"2\"   \"4\"   \"4\"    \"4\"   \"4\"    \"4\"    \"4\"    \"8\"    \"8\"     \"8\"      \"8\"    \"8\"     )"

######DEFINE SIZE OF PROBLEM#############
SIZE_START=1
SIZE_END=1
SIZE_MUL_X=2
SIZE_DIV_X=1
SIZE_MUL_Y=2
SIZE_DIV_Y=1
SIZE_MUL_Z=2
SIZE_DIV_Z=1
########################################

#####DEFINE PROCS NUMBER###############
PROC_START=2
PROC_END=${PROC_START}
######################################
CORE_START=10
CORE_END=${CORE_START}
#CORE_END=5
NUM_THREAD_PER_PROC=10
#FREQ_MODE 1 - change frequency: 0 - doen't change frequency
FREQ_MODE=1
FREQ_IDX_START=0
FREQ_IDX_END=0

PBS_NODES="node02"
PREFIX="MF_overhead_mf_mf_plugin_papi-mf_plugin_rapl-800msXXX_"
PREFIX+=$RANDOM
MV_COMM_START=5
MV_COMM_END=5
MV_ALGORITHM_START=1
MV_ALGORITHM_END=1
MV_ALGORITHM_OFF_DIAG_START=3
MV_ALGORITHM_OFF_DIAG_END=3

######DEFINE_BENCHMARK##########
#1    - benchmark matrix vector multiplication: (x=0; x=x+Ay)
#101  - benchmark matrix vector multiplication with barrier: (x=0; omp barrier; x=x+Ay)
#2    - benchmark vector operation with two loops: (   !vector_r = boundary_vector - vector_r; vector_d = vector_r )
#3    - benchmark norm 2 of vector (dot product)
#4    - solver cg
#5    - solver gmres
#10   - sleep 1 second
#11   - ten omp barrier in loop
BENCHMARK_START=1
BENCHMARK_END=${BENCHMARK_START}
################################

PRECONDITIONER_START=1
PRECONDITIONER_END=1
MATRIX_SCALER_START=1
MATRIX_SCALER_END=1



SUBMIT_COMM="qsub"
write_performance=1
write_profile=1
PERF_NEW=0

LUSTRE_DIR="$( echo $USER_LUSTRE_DIR_SED | sed  's/\\//g' )"
PROJ_DIR="$( echo $USER_PROJ_DIR_SED | sed  's/\\//g' )"

XX_STRING="$( echo $USER_XX | sed  's/\\//g' )"
eval "declare -a XX="${XX_STRING}
YY_STRING="$( echo $USER_YY | sed  's/\\//g' )"
eval "declare -a YY="${YY_STRING}
ZZ_STRING="$( echo $USER_ZZ | sed  's/\\//g' )"
eval "declare -a ZZ="${ZZ_STRING}

PROCS_ARRAY_STRING="$( echo $USER_PROCS_ARRAY | sed  's/\\//g' )"
eval "declare -a PROCS_ARRAY="${PROCS_ARRAY_STRING}
DX_STRING="$( echo $USER_DX | sed  's/\\//g' )"
eval "declare -a DX="${DX_STRING}
DY_STRING="$( echo $USER_DY | sed  's/\\//g' )"
eval "declare -a DX="${DY_STRING}
DZ_STRING="$( echo $USER_DZ | sed  's/\\//g' )"
eval "declare -a DX="${DZ_STRING}

COMM_ARRAY=("notused" "ISEND" "ISEND-GROUPS" "IBCAST-GROUPS" "ISEND-IDX" "ISEND-COMM-BUFER" )
MV_ALGORITHM_ARRAY=("notused" "MV_CSR" "MV_JAD" "MV_COO" )
BENCHMARK_ARRAY=("notused" "MV" "R-D" "NORM2" "CG" "GMRES")
PRECONDITIONER_ARRAY=("null" "JACOBI")
MATRIX_SCALER_ARRAY=("MATRIX-SCALER-NULL" "MATRIX-SCALER-DIAGONAL")
QSUB_REJECT_SLEEP=1
MAIN_SLEEP=1




for bench_idx in `seq ${BENCHMARK_START} ${BENCHMARK_END}`
do
for prec_idx in `seq ${PRECONDITIONER_START} ${PRECONDITIONER_END}`
do
for matscal_idx in `seq ${MATRIX_SCALER_START} ${MATRIX_SCALER_END}`
do
for mvalg_idx in `seq ${MV_ALGORITHM_START} ${MV_ALGORITHM_END}`
do
for mvalg_offd_idx in `seq ${MV_ALGORITHM_OFF_DIAG_START} ${MV_ALGORITHM_OFF_DIAG_END}`
do
for k in `seq ${MV_COMM_START} ${MV_COMM_END}`
do
XX_START=$(( ${XX[${SIZE_START}]}*${SIZE_MUL_X}/${SIZE_DIV_X} ))
XX_END=$(( ${XX[${SIZE_END}]}*${SIZE_MUL_X}/${SIZE_DIV_X} ))
YY_START=$(( ${YY[${SIZE_START}]}*${SIZE_MUL_Y}/${SIZE_DIV_Y} ))
YY_END=$(( ${YY[${SIZE_END}]}*${SIZE_MUL_Y}/${SIZE_DIV_Y} ))
ZZ_START=$(( ${ZZ[${SIZE_START}]}*${SIZE_MUL_Z}/${SIZE_DIV_Z} ))
ZZ_END=$(( ${ZZ[${SIZE_END}]}*${SIZE_MUL_Z}/${SIZE_DIV_Z} ))

file_name_part="${PREFIX}_${BENCHMARK_ARRAY[bench_idx]}_${PRECONDITIONER_ARRAY[prec_idx]}_${MATRIX_SCALER_ARRAY[matscal_idx]}_mv_comm_${k}_mv_alg_${MV_ALGORITHM_ARRAY[mvalg_idx]}_${MV_ALGORITHM_ARRAY[mvalg_offd_idx]}_size_${XX_START}_${XX_END}x${YY_START}_${YY_END}x${ZZ_START}_${ZZ_END}_procs_${PROCS_ARRAY[${PROC_START}]}_${PROCS_ARRAY[${PROC_START}]}_freqmode_${FREQ_MODE}_freq_${FREQ_IDX_START}-${FREQ_IDX_END}_core_${CORE_START}_${CORE_END}"
eval "concatenate_file="${LUSTRE_DIR}/concatenate_${file_name_part}.sh
result_file_name=result_${file_name_part}.dat
eval "result_file="${LUSTRE_DIR}/${result_file_name}
if [ -z "$result_file_name" ]; then
 echo "ERROR: check result_file_name"
 exit 8
fi
if [ -z "$result_file" ]; then
 echo "ERROR: check result_file"
 exit 9
fi
if [ "${PERF_NEW}" -ge "1" ];  then
  rm ${result_file} -rf
fi
touch ${concatenate_file}
touch ${result_file}

prop=${file_name_part}
perf_file="${prop}.dat"
prof_file="${prop}_prof.dat"

gnu_file="${file_name_part}.gnuplot"
gnu_eps_file="${file_name_part}.eps"
do_gnu_file=${PROJ_DIR}/scripts/${file_name_part}_gnuplot.sh
gnu_plot_title="${BENCHMARK_ARRAY[bench_idx]};${COMM_ARRAY[${k}]};domain:${XX_START}-${XX_END}x${YY_START}-${YY_END}x${ZZ_START}-${ZZ_END} procs:${PROCS_ARRAY[${PROC_START}]}-${PROCS_ARRAY[${PROC_END}]};freq:${FREQ_IDX_START}-${FREQ_IDX_END};cores:${CORE_START}-${CORE_END}"
cp ${PROJ_DIR}/performance_gnuplot.template ${do_gnu_file}

sed_string="s/USER_PROJ_DIR/${USER_PROJ_DIR_SED}/g"
sed -i "${sed_string}" ${do_gnu_file}
sed_string="s/USER_LUSTRE_DIR/${USER_LUSTRE_DIR_SED}/g"
sed -i "${sed_string}" ${do_gnu_file}
sed_string="s/USER_GNUPLOT_FILE/${gnu_file}/g"
sed -i "${sed_string}" ${do_gnu_file}
sed_string="s/USER_PS_FILE/${gnu_eps_file}/g"
sed -i "${sed_string}" ${do_gnu_file}
sed_string="s/USER_RESULT_FILE/${result_file_name}/g"
sed -i "${sed_string}" ${do_gnu_file}
sed_string="s/USER_TITLE/${gnu_plot_title}/g"
sed -i "${sed_string}" ${do_gnu_file}

 for j in `seq ${SIZE_START} ${SIZE_END}`
 do

  eval "run_file="${PROJ_DIR}/scripts/run_${file_name_part}_indx_${j}.sh
  run_file_sed="${USER_PROJ_DIR_SED}\/scripts\/run_${file_name_part}_indx_${j}.sh"
  cp run_experiment_hornet.sh ${run_file}

  sed_string="s/USER_PROJ_DIR/${USER_PROJ_DIR_SED}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_BIN/${USER_BIN_SED}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_BIN_PAT/${USER_BIN_PAD_SED}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_LUSTRE_DIR/${USER_LUSTRE_DIR_SED}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_TOL_EP/${USER_TOL_EP}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_XX/${USER_XX}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_YY/${USER_YY}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_ZZ/${USER_ZZ}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_DX/${USER_DX}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_DY/${USER_DY}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_DZ/${USER_DZ}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_PROCS_ARRAY/${USER_PROCS_ARRAY}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_VERBOSITY/${USER_VERBOSITY}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_RUN_ATTEMPTS/${USER_RUN_ATTEMPTS}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_RUN_APRUN_PARM_1/${USER_RUN_APRUN_PARM_1}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_RUN_APRUN_PARM_2/${USER_RUN_APRUN_PARM_2}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_RUN_APRUN_PARM_N/${USER_RUN_APRUN_PARM_N}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_RUN_CHECK_RESULT/${USER_RUN_CHECK_RESULT}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_TOL_EPS/${TOL_EP}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/user_write_performance/${write_performance}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/user_write_profile/${write_profile}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_ITER_MAX/${ITER_MAX}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_SIZE_MUL_X/${SIZE_MUL_X}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_SIZE_DIV_X/${SIZE_DIV_X}/g"
  sed -i "${sed_string}" ${run_file}
   sed_string="s/USER_SIZE_MUL_Y/${SIZE_MUL_Y}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_SIZE_DIV_Y/${SIZE_DIV_Y}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_SIZE_MUL_Z/${SIZE_MUL_Z}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_SIZE_DIV_Z/${SIZE_DIV_Z}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_NUM_MASTERS_PER_NODE/${USER_NUM_MASTERS_PER_NODE}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_NUM_MASTERS_PER_2NODES/${USER_NUM_MASTERS_PER_2NODES}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_FREQ_MODE/${FREQ_MODE}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_FREQ_IDX_START/${FREQ_IDX_START}/g"
  sed -i "${sed_string}" ${run_file}
  sed_string="s/USER_FREQ_IDX_END/${FREQ_IDX_END}/g"
  sed -i "${sed_string}" ${run_file}




#  for p in `seq ${PROC_START} ${PROC_END}`
#  do
   for i in `seq ${CORE_START} ${CORE_END}`
   do
     let p=${PROC_START}
     eval "job_file_name="${PROJ_DIR}/scripts/job_${prop}_idx_${j}.sh
     cp job_on_hornet.template  ${job_file_name}

     echo "prepare job:"  ${job_file_name}

     let JOB_WIDTH=${PROCS_ARRAY[p]}*${NUM_THREAD_PER_PROC}

     sed_string="s/USER_RUN_SCRIPT_NAME/${run_file_sed}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_JOB_WIDTH/${JOB_WIDTH}/g"
     sed -i "${sed_string}" ${job_file_name}


     sed_string="s/USER_PROC_START/${p}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_PBS_NODES/${PBS_NODES}/g"
     sed -i "${sed_string}" ${job_file_name}


     sed_string="s/USER_MV_ALGORITHM/${mvalg_idx}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/user_matrix_scaler/${matscal_idx}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/user_mv_algorithm_off_diag/${mvalg_offd_idx}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_PROC_END/${p}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_CORE_START/${i}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_CORE_END/${i}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_SIZE_START/${j}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_SIZE_END/${j}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_PREFIX/${PREFIX}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_PERF_NEW/${PERF_NEW}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_PROJ_DIR/${USER_PROJ_DIR_SED}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_LUSTRE_DIR/${USER_LUSTRE_DIR_SED}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_OUT/${prop}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_BENCHMARK/${bench_idx}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/user_preconditioner/${prec_idx}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_ERR/${prop}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_JOB_NAME/${prop}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_MV_COMMUNICATOR/${k}/g"
     sed -i "${sed_string}" ${job_file_name}

     sed_string="s/USER_PERF_FILE/${perf_file}/g"
     sed -i "${sed_string}" ${job_file_name}
     sed_string="s/USER_PROF_FILE/${prof_file}/g"
     sed -i "${sed_string}" ${job_file_name}

     DOMAIN=".fe.excess-project.eu"
     PBS_JOBID=$( ${SUBMIT_COMM} ${job_file_name} )
     PBS_JOBID=${PBS_JOBID%${DOMAIN}}
     echo "PBS_JOBID:"${PBS_JOBID}

     # update configuration
     CONFIG_FILE=/nas_home/hpcdhopp/cel_job/mv/overhead/configurations/mf_config-mf_plugin_papi-mf_plugin_rapl-800ms.ini
     HOME_USER=/nas_home/${USER}
     MF_USER_TOP_PATH=${HOME_USER}/.mf
     MF_AGENT_USER_CONFIGFILE=${MF_USER_TOP_PATH}/${PBS_JOBID}.ini
     echo "Copy new configuration to "${MF_AGENT_USER_CONFIGFILE}
     echo "Config file is: "${CONFIG_FILE}
     cp -f ${CONFIG_FILE} ${MF_AGENT_USER_CONFIGFILE}

     #while [ $? -ne 0 ]; do
     #  echo "could not submit the job" ${job_file_name}
     #  echo "will try in ${QSUB_REJECT_SLEEP} minutes"
     #  sleep ${QSUB_REJECT_SLEEP}
     #  ${SUBMIT_COMM} ${job_file_name}
     #done
     echo "job submitted: " ${job_file_name}


     sleep 1
   done
   echo "sleep ${MAIN_SLEEP} seconds"
   sleep ${MAIN_SLEEP}
 done
done
  echo "cat ${LUSTRE_DIR}/${perf_file} >> ${result_file}" >> ${concatenate_file}
  echo "source ${do_gnu_file}">>${concatenate_file}
  echo "gnuplot ${PROJ_DIR}/scripts/${gnu_file}" >>${concatenate_file}
done
done
done
done
done
