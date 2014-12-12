#!/bin/sh
pwd=`pwd`

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$pwd/papi/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$pwd/../client/lib
