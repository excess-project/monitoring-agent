#!/bin/bash

echo "Activating monitoring framework"
mkdir -p $HOME/.mf/service/node03
mkdir -p $HOME/.mf/service/node02
mkdir -p $HOME/.mf/service/node01
touch $HOME/.mf/service/node03/start_monitoring
touch $HOME/.mf/service/node02/start_monitoring
touch $HOME/.mf/service/node01/start_monitoring
