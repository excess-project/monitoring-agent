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

BEGIN {
    FS=" "
}
(NF==6 && !/Device/ && !/d/) {
    current_time=""
    "date +'%Y-%m-%d'" |& getline current_time".123";
    close("date +'%Y-%m-%d'")

    epoch_time=""
    "date +'%s'" |& getline epoch_time
    close("date +'%s'")

    "hostname" |& getline hostname

    JSON="{\"Timestamp\":\""epoch_time"\", "
    JSON=JSON"\"type\":\"iostat:avgcpu\", "
    JSON=JSON"\"hostname\":\""hostname"\", "
    JSON=JSON"\"iostat:avgcpu:user\":"$1", "
    JSON=JSON"\"iostat:avgcpu:nice\":"$2", "
    JSON=JSON"\"iostat:avgcpu:system\":"$3", "
    JSON=JSON"\"iostat:avgcpu:iowait\":"$4", "
    JSON=JSON"\"iostat:avgcpu:steal\":"$5", "
    JSON=JSON"\"iostat:avgcpu:idle\":"$6" "
    JSON=JSON"}"
    print JSON

    URL=server"/"id
    "curl -i -H 'Accept: application/json' -H 'Content-Type:application/json' -X POST "URL " --data '"JSON"'" |& getline results
    print results
}
END {
    print "Done."
}