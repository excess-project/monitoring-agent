#!/bin/bash
EXECUTION_ID=$1
MF_SERVER=$2

iostat -m 1 | awk -v server=$2 -v id=$1 -f iostat.awk
