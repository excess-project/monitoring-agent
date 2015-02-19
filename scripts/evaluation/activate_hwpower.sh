#!/bin/bash

ACTIVATE=$1

if [ $ACTIVATE -eq 0 ]; then
  echo "Deactivating power measurement"
  rm -f $HOME/.pwm/node02/copy_raw_data
else
  echo "Activating power measurement"
  mkdir -p $HOME/.pwm/node02/
  touch $HOME/.pwm/node02/copy_raw_data
fi
