#!/bin/sh
pwd=`pwd`
echo $pwd 
papi="/papi/lib"

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$pwd$papi
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$pwd/../client/contrib/publisher
