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
Usage: $0 [i:c:h]

-h                  prints this usage information.

-i <ID>             sets a user-defined experiment ID
-c <CONFIG_FILE>    specifies a user-defined configuration file (e.g., mf_config.ini)
EOF

}

BASE_DIR=`pwd`
DIST_DIR=${BASE_DIR}/dist
DIST_BIN_DIR=${DIST_DIR}/bin
LIB_DIR=${DIST_DIR}/lib

unset ID
unset CONFIG

while getopts ":i:c:h" opt; do
  case $opt in
    i)
      ID=$OPTARG
      ;;
    c)
      CONFIG=$OPTARG
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

if [ $OPTIND -eq 1 ]; then
    echo "Starting agent..." >&2
    nohup ${DIST_BIN_DIR}/mf_agent > mf_agent.log 2>&1&
    echo $! > mf_agent.pid
    echo "Done."
    echo
    exit 0;
fi

# start monitoring agent with appropriate parameters
echo "Starting agent..." >&2
if [ ! -z "$ID" ]; then
    if [ ! -z "$CONFIG" ]; then
        nohup ${DIST_BIN_DIR}/mf_agent -id=$ID -config=$CONFIG > mf_agent.log 2>&1&
        echo $! > mf_agent.pid
        echo "Done."
        echo
        exit 0;
    fi
    nohup ${DIST_BIN_DIR}/mf_agent -id=$ID > mf_agent.log 2>&1&
    echo $! > mf_agent.pid
    echo "Done."
    echo
    exit 0;
fi

if [ ! -z "$CONFIG" ]; then
    nohup ${DIST_BIN_DIR}/mf_agent -config=$CONFIG > mf_agent.log 2>&1&
    echo $! > mf_agent.pid
fi
echo "Done."
echo

# end