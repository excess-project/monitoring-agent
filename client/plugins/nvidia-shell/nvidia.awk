#!/bin/bash
#  Copyright 2015 High Performance Computing Center, Stuttgart
#  Adapted for nvidia-smi by Anders Gidenstam 2015.
#  Copyright 2015 Anders Gidenstam, Chalmers University of Technology.
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

BEGIN {
    gpu_id=-1
}
(NR==1) {
    epoch_time=$0
}
# Collect interesting information from the nvidia-smi output.
# NOTE: This is fragile as it is strongly dependent on the nvidia-smi output.
/^GPU/ {
    gpu_id+=1
}
/GPU UUID/ {
    GPU[gpu_id, "UUID"] = $4
}
/GPU Current Temp/ {
    GPU[gpu_id, "temperature"] = $5
}
/Power Draw/ {
    GPU[gpu_id, "power"] = $4
}
#   Utilization (NOTE: The key needs to be unique in the output.)
/Gpu/ {
    GPU[gpu_id, "GPUutilization"] = $3
}
END {
    "hostname" |& getline hostname

    JSON=""
    JSON=JSON"{\"Timestamp\":\""epoch_time"\", "
    JSON=JSON"\"type\":\"power\", "
    JSON=JSON"\"hostname\":\""hostname"\""

    for (i=0; i <= gpu_id; i++) {
        #JSON=JSON", \"GPU"i":UUID\":\""GPU[i, "UUID"]"\""
        JSON=JSON", \"GPU"i":power\":"GPU[i, "power"]""
        JSON=JSON", \"GPU"i":temperature\":"GPU[i, "temperature"]""
        JSON=JSON", \"GPU"i":GPUutilization\":"GPU[i, "GPUutilization"]""
    }
    JSON=JSON"}"
    print JSON

    URL=server"/"id
    "curl -i -s -H 'Accept: application/json' -H 'Content-Type:application/json' -X POST "URL " --data '"JSON"'" |& getline results

    print "Done with result:" results
}
