#!/bin/sh
curl=`which curl`
if [ -z "$curl" ]; then
    echo "Please install the 'curl' package before running this script."
    exit 1
fi

HOST="localhost"
HOST="141.58.5.220"
if [ -n "$1" ]; then
    HOST=$1   
fi

# NOTE: the UMU 3 server uses 8087 as the port number due to Emotive installation
PORT=3000
if [ -n "$2" ]; then
    PORT=$2
fi
echo "Running this script on" $HOST "with port" $PORT


ID=`curl -s -X POST -H 'Content-Type: application/json' -d '{"Name":"Execution1","Description": "Test", "nodes":["node1","node2","node3"]}' http://${HOST}:$PORT/executions`
echo curl -s -X POST -H 'Content-Type: application/json' -d '{"Name":"Execution1","Description": "Test", "nodes":["node1","node2","node3"]}' http://${HOST}:$PORT/executions
echo "ID = $ID"

echo
echo curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1398169317","cpu":"0.55","ram":"125","metric_3":"value3","metric_4":"value4"}' http://$HOST:$PORT/executions/$ID
curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp":"1398169317","cpu":"0.55","ram":"125","metric_3":"value3","metric_4":"value4"}' http://$HOST:$PORT/executions/$ID

sleep 1
echo
newID=`echo $ID | tr [A-Z] [a-z]`
echo curl -i -X GET http://$HOST:9200/$newID/_search?pretty=true
curl -i -X GET http://$HOST:9200/$newID/_search?pretty=true

echo

