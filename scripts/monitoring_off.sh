#!/bin/bash

echo "Deactivating monitoring framework"
rm -f $HOME/.mf/service/node03/start_monitoring
rm -f $HOME/.mf/service/node02/start_monitoring
rm -f $HOME/.mf/service/node01/start_monitoring