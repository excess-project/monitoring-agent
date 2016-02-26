#!/bin/bash
#  Copyright (C) 2015 University of Stuttgart
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

echo "Stopping services ..."
echo "> mf_agent"
if [ -f "mf_agent.pid" ]
then
    PID=$(cat mf_agent.pid)
    kill ${PID}
    rm -f mf_agent.pid
else
    echo "Couldn't find PID associated with mf_agent process."
    echo "Please kill the service manually."
fi
echo "Done."
echo