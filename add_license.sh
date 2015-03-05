#!/bin/bash

FILES=$( find client -name '*.sh' )
for i in $FILES
do
  if ! grep -q Copyright $i
  then
    #echo $i
    cat template $i >$i.new && mv $i.new $i
  fi
done
