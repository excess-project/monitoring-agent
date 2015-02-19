#!/bin/bash

ACTIVATE=$1

if [ $ACTIVATE -eq 0 ]; then
  echo "Deactivating monitoring framework"
  rm -f $HOME/.mf/service/node02/start_monitoring
  rm -f $HOME/.mf/service/node01/start_monitoring
else
  echo "Activating monitoring framework"
  mkdir -p $HOME/.mf/service/node02
  mkdir -p $HOME/.mf/service/node01
  touch $HOME/.mf/service/node02/start_monitoring
  touch $HOME/.mf/service/node01/start_monitoring
fi
