#!/bin/bash

source setenv.sh
sudo env LD_LIBRARY_PATH=$LD_LIBRARY_PATH ./mf_infiniband_client "$@"