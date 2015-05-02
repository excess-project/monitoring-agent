#!/bin/bash

#  Awk utility to format the output of the iostat command for import to spreadsheet
#  Usage 1:  iostat x y | awk -f iostat.awk
#  Usage 2:  iostat x y > iostat.out
#            awk -f iostat.awk iostat.out
#  Comment:  this utility is designed to work with any number of disks
#  Bruce Spencer, IBM  11-25-98
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

    JSON="{\"timestamp\":\""epoch_time"\", "
    JSON=JSON"\"type\":\"iostat:avgcpu\" ,"
    JSON=JSON"\"iostat:avgcpu:user\":"$1" ,"
    JSON=JSON"\"iostat:avgcpu:nice\":"$2" ,"
    JSON=JSON"\"iostat:avgcpu:system\":"$3" ,"
    JSON=JSON"\"iostat:avgcpu:iowait\":"$4" ,"
    JSON=JSON"\"iostat:avgcpu:steal\":"$5" ,"
    JSON=JSON"\"iostat:avgcpu:idle\":"$6" "
    JSON=JSON"}"
    print JSON

    #URL=server"/"tolower(workflow)"_"tolower(task)"_"current_time"/"id;
    URL=server"/executions"
    "curl -XGET "URL |& getline results
    print results
}
END {
    print "Done."
}