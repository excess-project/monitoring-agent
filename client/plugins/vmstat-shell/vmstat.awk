#!/bin/bash
BEGIN {
    FS=" "
}
(NF==2) {
    epoch_time=""
    "date +'%s'" |& getline epoch_time
    close("date +'%s'")

    "hostname" |& getline hostname

    JSON="{\"Timestamp\":\""epoch_time"\", "
    JSON=JSON"\"type\":\"statistics\", "
    JSON=JSON"\"hostname\":\""hostname"\", "
    JSON=JSON"\""$1"\":"$2""
    JSON=JSON"}"

    "curl -i -s -H 'Accept: application/json' -H 'Content-Type:application/json' -X POST "server " --data '"JSON"'" |& getline results
}
END {
    print "Done."
}