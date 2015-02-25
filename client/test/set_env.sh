/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#!/bin/bash
PWD=`pwd`

TOP_PATH=../../binaries
PAPI=$TOP_PATH/papi/lib
LIKWID=$TOP_PATH/likwid/lib

OLD_PATH=$LD_LIBRARY_PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/$PAPI
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/$LIKWID