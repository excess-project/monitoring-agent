#!/bin/bash
if [ -z $1 ]; then 
	Number_of_itterations=100
else
	Number_of_itterations=$1
fi
if [ -z $2 ]; then 
	When=`date '+%d/%m/%y-%H:%M'`
	ID=`curl -i  -X POST -H 'Content-Type: application/json' -d '{"Name": "Execution -'$When'", "Description": "Testing ('$Number_of_itterations') iterations","Other":"values","Onemore":"please"}' http://localhost:3000/executions | tail -1 2>/dev/null`
	echo "New ID has been created " $ID
else
	ID=$2
fi

for (( c=1; c<=$Number_of_itterations; c++ ))
do
	TimeStamp=`date '+%s'`
	Memory=`top -b -n1 | grep Mem: | awk '{print $2}'`
	Sys_CPU=` top -b -n1 | grep 'Cpu' | awk '{print $3}'`
	User_CPU=`top -b -n1 | grep 'Cpu' | awk '{print $2}'`
	Sys_CPU=${Sys_CPU%"%sy,"}
	User_CPU=${User_CPU%"%us,"}
	Memory=${Memory%"k"}
	curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp": '$TimeStamp', "User_CPU": '$User_CPU',"Sys_CPU": '$Sys_CPU',"Memory": '$Memory'}' http://localhost:3000/executions/$ID
        //echo curl -i -X POST -H 'Content-Type: application/json' -d '{"Timestamp": '$TimeStamp', "User_CPU": '$User_CPU',"Sys_CPU": '$Sys_CPU',"Memory": '$Memory'}' http://localhost:3000/executions/$ID
done
