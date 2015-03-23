#!/bin/bash
EXECUTION_ID=$1
MF_SERVER=$2

### check URL ###
MF_SERVER=${MF_SERVER%/}
URL=$MF_SERVER"/"$EXECUTION_ID
CURL_CMD=$( curl -s -S -XGET $URL )
echo $CURL_CMD
if [[ "curl" == ${CURL_CMD}* ]]; then
    exit 1
fi
case "$CURL_CMD" in
  *Cannot*     ) exit 1 ;;
esac

iostat -m 1 | awk -v server=$2 -v id=$1 -f iostat.awk
