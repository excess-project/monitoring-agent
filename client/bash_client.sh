#!/bin/bash
if [ -z $1 ]; then 
	Number_of_itterations=100
else
	Number_of_itterations=$1
fi
if [ -z $2 ]; then 
	When=`date '+%d/%m/%y-%H:%M'`
#	When="testing"
	echo $When
	ID=`curl -i  -s -X POST -H 'Content-Type: application/json' -d '{"Name": "Execution-'$When'", "Description": "Excess Review Execution","other":"Other needed info","Nodes":"My own Laptop"}' http://localhost:3000/executions | tail -1 2>/dev/null`
	echo "New ID has been created " $ID
	echo $ID
else
	ID=$2
fi

for (( c=1; c<=$Number_of_itterations; c++ ))
do
	TimeStamp=`date '+%s'`
	Memory=`top -l 1 | grep PhysMem: | awk '{print $2}'`
	Sys_CPU=`top -l 1 | grep 'CPU usage' | awk '{print $5}'`
	User_CPU=`top -l 1 | grep 'CPU usage' | awk '{print $3}'`
	Sys_CPU=${Sys_CPU%"%"}
	User_CPU=${User_CPU%"%"}
	Memory=${Memory%"M"}
	curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp": '$TimeStamp', "User_CPU": '$User_CPU',"Sys_CPU": '$Sys_CPU',"Memory": '$Memory'}' http://localhost:3000/executions/$ID
	echo "curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp": '$TimeStamp', "User_CPU": '$User_CPU',"Sys_CPU": '$Sys_CPU',"Memory": '$Memory'}' http://localhost:3000/executions/$ID"
	echo "ThisismyID$ID"
done
