#!/bin/bash
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

    URL=server""id
    "curl -i -s -H 'Accept: application/json' -H 'Content-Type:application/json' -X POST "URL " --data '"JSON"'" |& getline results
}
(NF==6 && !/Device/ && /d/) {
    current_time=""
    "date +'%Y-%m-%d'" |& getline current_time".123";
    close("date +'%Y-%m-%d'")

    epoch_time=""
    "date +'%s'" |& getline epoch_time
    close("date +'%s'")

    "hostname" |& getline hostname

    JSON="{\"Timestamp\":\""epoch_time"\", "
    JSON=JSON"\"type\":\"iostat:"$1"\", "
    JSON=JSON"\"hostname\":\""hostname"\", "
    JSON=JSON"\"iostat:"$1":tps\":"$2", "
    JSON=JSON"\"iostat:"$1":MS_read/s\":"$3", "
    JSON=JSON"\"iostat:"$1":MB_wrtn/s\":"$4", "
    JSON=JSON"\"iostat:"S1":MB_read\":"$5", "
    JSON=JSON"\"iostat:"$1":MS_wrtn\":"$6" "
    JSON=JSON"}"

    URL=server""id
    "curl -i -s -H 'Accept: application/json' -H 'Content-Type:application/json' -X POST "URL " --data '"JSON"'" |& getline results
}
END {
    print "Done."
}