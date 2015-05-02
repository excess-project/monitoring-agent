#!/bin/bash
EXECUTION_ID=$1
MF_SERVER=$2

### check URL ###
MF_SERVER=${MF_SERVER%/}
URL=$MF_SERVER"/"$EXECUTION_ID
curl -s --head $URL | head -n 1 | grep "HTTP/1.[01] [23].." > /dev/null
if [[ $? -eq "1" ]]; then
  echo "Error: Given URL ${URL} not reachable."
  exit 1
fi

while sleep 2; do cat /proc/vmstat | awk -v server=$URL -f mf_plugin_vmstat.awk; done

