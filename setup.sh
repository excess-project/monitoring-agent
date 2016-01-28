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

# ============================================================================ #
# DEFAULT PATHS                                                                #
# ============================================================================ #

ROOT=`pwd`
BINARY_FOLDER="bin"
INSTALL_PATH_PAPI=`pwd`/${BINARY_FOLDER}/papi
INSTALL_PATH_LIKWID=`pwd`/${BINARY_FOLDER}/likwid
INSTALL_PATH_APR=`pwd`/${BINARY_FOLDER}/apr
INSTALL_PATH_APU=${INSTALL_PATH_APR}
INSTALL_PATH_CURL=`pwd`/${BINARY_FOLDER}/curl
INSTALL_PATH_NVIDIA=`pwd`/${BINARY_FOLDER}/nvidia

# ============================================================================ #
# VERSIONS OF REQUIRED LIBRARIES                                               #
# ============================================================================ #
PAPI="papi-5.4.0"

# ============================================================================ #
# DOWNLOAD AND INSTALL PAPI-C                                                  #
# ============================================================================ #

cd $ROOT
wget http://icl.cs.utk.edu/projects/papi/downloads/${PAPI}.tar.gz
tar zxvf ${PAPI}.tar.gz
cd ${PAPI}/src
./configure --prefix=${INSTALL_PATH_PAPI} --with-components="rapl coretemp infiniband"
make
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL LIKWID                                                  #
# ============================================================================ #

cd $ROOT
wget http://ftp.fau.de/pub/likwid/likwid-3.1.3.tar.gz
tar zxvf likwid-3.1.3.tar.gz
cd likwid-3.1.3
sed -i 's/^SHARED_LIBRARY\s=\sfalse/SHARED_LIBRARY = true/' config.mk
sed -i -e 's@^PREFIX\s=\s/usr/local@PREFIX = '"$INSTALL_PATH_LIKWID"'@' config.mk
make
sed -i 's/^SHARED_LIBRARY\s=\strue/SHARED_LIBRARY = false/' config.mk
make
make install

# ============================================================================ #
# DOWNLOAD AND INSTALL CURL                                                    #
# ============================================================================ #

cd $ROOT
wget http://curl.haxx.se/download/curl-7.37.0.tar.gz
tar zxvf curl-7.37.0.tar.gz
cd curl-7.37.0
./configure --prefix=${INSTALL_PATH_CURL}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL APACHE APR                                              #
# ============================================================================ #

cd $ROOT
wget http://mirror.23media.de/apache/apr/apr-1.5.1.tar.gz
tar zxvf apr-1.5.1.tar.gz
cd apr-1.5.1
./configure --prefix=${INSTALL_PATH_APR}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL APACHE APR UTILITIES                                    #
# ============================================================================ #

cd $ROOT
wget http://mirror.23media.de/apache/apr/apr-util-1.5.3.tar.gz
tar zxvf apr-util-1.5.3.tar.gz
cd apr-util-1.5.3
./configure --prefix=${INSTALL_PATH_APU} --with-apr=${INSTALL_PATH_APR}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL NVIDIA GDK                                              #
# ============================================================================ #

cd $ROOT
mkdir nvidia_gdk_download
cd nvidia_gdk_download
wget http://developer.download.nvidia.com/compute/cuda/7.5/Prod/gdk/gdk_linux_amd64_352_55_release.run
chmod +x gdk_linux_amd64_352_55_release.run
./gdk_linux_amd64_352_55_release.run --silent --installdir=${INSTALL_PATH_NVIDIA}

# ============================================================================ #
# CLEANING UP                                                                  #
# ============================================================================ #

cd $ROOT
rm -f *.tar.gz
rm -rf papi-5.4.0
rm -rf likwid-3.1.3
rm -rf apr-1.5.1
rm -rf apr-util-1.5.3
rm -rf curl-7.37.0
rm -rf nvidia_gdk_download

# ============================================================================ #
# COMPILING                                                                    #
# ============================================================================ #

cd $ROOT
make
make install