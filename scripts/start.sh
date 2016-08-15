#!/bin/bash
#  Copyright (C) 2015 University of Stuttgart
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

usage() {
    cat <<EOF
Usage: $0 [w:t:c:e:o:a:h]

-h                  prints this usage information.

-w <USER_ID>        sets a current user (EXCESS) or workflow (DreamCloud) ID [optional]
-t <TASK_ID>        sets a user-defined application (EXCESS) or task (DreamCloud) ID [optional]
-e <ID>             sets a user-defined experiment ID [optional]
-o <HOSTNAME>       sets a user-defined hostname [optional]
-a <API_VERSION>    sets the API version to communicate with the backend (default: v1) [optional]
-c <CONFIG_FILE>    specifies a user-defined configuration file (e.g., mf_config.ini) [optional]
EOF

}

BASE_DIR=`pwd`
DIST_DIR=${BASE_DIR}
DIST_BIN_DIR=${DIST_DIR}/bin
LIB_DIR=${DIST_DIR}/lib

PARAMS=''

while getopts "w:t:c:e:o:a:h" opt; do
  case $opt in
    w)
      PARAMS+=" -w ${OPTARG}"
      ;;
    t)
      PARAMS+=" -t ${OPTARG}"
      ;;
    c)
      PARAMS+=" -c ${OPTARG}"
      ;;
    e)
      PARAMS+=" -e ${OPTARG}"
      ;;
    o)
      PARAMS+=" -o ${OPTARG}"
      ;;
    a)
      PARAMS+=" -a ${OPTARG}"
      ;;
    h)
      usage
      exit 0
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

# set environment variables
libs=${LIB_DIR}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libs

# run agent with given parameters
nohup ${DIST_BIN_DIR}/mf_agent ${PARAMS} > mf_agent.log 2>&1&
echo $! > mf_agent.pid
echo "Done."
echo

# end