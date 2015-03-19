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
    print JSON

    #URL=server"/"tolower(workflow)"_"tolower(task)"_"current_time"/"id;
    URL=server"/"id
    "curl -i -H 'Accept: application/json' -H 'Content-Type:application/json' -X POST "URL " --data '"JSON"'" |& getline results
    print results
}
END {
    print "Done."
}