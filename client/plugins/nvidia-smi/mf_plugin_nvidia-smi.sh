#!/bin/bash
#  Copyright 2015 High Performance Computing Center, Stuttgart
#  Adapted for nvidia-smi by Anders Gidenstam 2015.
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

EXECUTION_ID=$1
MF_SERVER=$2

### check URL ###
MF_SERVER=${MF_SERVER%/}
URL=$MF_SERVER"/"$EXECUTION_ID
curl -s --head $URL | head -n 1 | grep "HTTP/1.[01] [23].." > /dev/null
if [[ $? -eq "1" ]]; then
    echo "Error: Given URL ${URL} not reachable."
    exit 1
fi

while sleep 1; do
    (date '+%s.%N'; nvidia-smi -q) | awk -v server=$URL -f mf_plugin_nvidia-smi.awk;
done
