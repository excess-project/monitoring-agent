#!/bin/bash


URL="http://localhost:3000"
EXECUTIONS="${URL}/executions"
CONTENT_TYPE="-H \"Content-Type: application/json\""

CURL_CMD="curl"

now=$(date +"%m-%d-%Y")
EXECUTION_ID=$( curl -X POST -H "Content-Type: application/json" \
--data '{ "Name":"winterfell", "Description":"Manual test with User-Library '"$now"'", "Start_date":"Wed Mar 4 10:37:39 2015", "Username": "hpcdhopp" }' \
http://localhost:9200/executions/TBD )

echo $EXECUTION_ID

EXECUTION_ID=$( echo $EXECUTION_ID | awk -F'"' '{ print $12 }' )
LOWERCASE_ID=$( echo $EXECUTION_ID | awk '{ print tolower($0) }' )

curl -X PUT http://localhost:9200/$LOWERCASE_ID

curl -X POST -H "Content-Type: application/json" \
--data '{ "Timestamp": "2015-03-05-09:36:00", "user": "hpcdhopp", "type": "test", "PAPI_TOT_INS": 0 }' \
http://localhost:9200/$LOWERCASE_ID/TBD

#http://localhost:9200/auvpaqeb4k3srigiwtjw/_search?pretty=true

sleep 1

echo $EXECUTION_ID
echo "http://localhost:9200/${LOWERCASE_ID}/_search?pretty=true"
#RESPONSE=$( curl -X GET "http://localhost:3000/executions/${EXECUTION_ID}" )
#curl -X GET "http://localhost:9200/${LOWERCASE_ID}/_refresh"
RESPONSE=$( curl -X GET "http://localhost:9200/${LOWERCASE_ID}/_search" )

echo $RESPONSE


curl -X POST -H "Content-Type: application/json" \
--data '{ "Timestamp": "2015-03-05-09:36:00", "user": "hpcdhopp", "type": "test", "PAPI_TOT_INS": 0 }' \
http://localhost:9200/$LOWERCASE_ID/TBD

curl -X POST -H "Content-Type: application/json" \
--data '{ "Timestamp": "2015-03-05-09:36:00", "user": "hpcdhopp", "type": "test", "PAPI_TOT_INS": 0 }' \
http://localhost:9200/$LOWERCASE_ID/TBD

echo $EXECUTION_ID
echo "http://localhost:9200/${LOWERCASE_ID}/_search?pretty=true"
#RESPONSE=$( curl -X GET "http://localhost:3000/executions/${EXECUTION_ID}" )
curl -X GET "http://localhost:9200/${LOWERCASE_ID}/_refresh"
RESPONSE=$( curl -X GET "http://localhost:9200/${LOWERCASE_ID}/_search" )

echo $RESPONSE
