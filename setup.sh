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
INSTALL_PATH_MOVIDIUS_ARDUINO=`pwd`/${BINARY_FOLDER}/movidius_arduino
INSTALL_PATH_LIKWID=`pwd`/${BINARY_FOLDER}/likwid
INSTALL_PATH_APR=`pwd`/${BINARY_FOLDER}/apr
INSTALL_PATH_APU=${INSTALL_PATH_APR}
INSTALL_PATH_CURL=`pwd`/${BINARY_FOLDER}/curl
INSTALL_PATH_NVIDIA=`pwd`/${BINARY_FOLDER}/nvidia
INSTALL_PATH_BISON=`pwd`/${BINARY_FOLDER}/bison
INSTALL_PATH_FLEX=`pwd`/${BINARY_FOLDER}/flex
INSTALL_PATH_SENSORS=`pwd`/${BINARY_FOLDER}/sensors


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
wget http://www.eu.apache.org/dist//apr/apr-1.5.2.tar.gz
tar zxvf apr-1.5.2.tar.gz
cd apr-1.5.2
./configure --prefix=${INSTALL_PATH_APR}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL APACHE APR UTILITIES                                    #
# ============================================================================ #

cd $ROOT
wget http://www.eu.apache.org/dist//apr/apr-util-1.5.4.tar.gz
tar zxvf apr-util-1.5.4.tar.gz
cd apr-util-1.5.4
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
# DOWNLOAD AND INSTALL SENSORS LIB                                             #
# https://fossies.org/linux/misc/lm_sensors-3.4.0.tar.gz/lm_sensors-3.4.0/lib/libsensors.3
# ============================================================================ #
#
# DEPENDENCIES: bison and flex
#
cd $ROOT
wget http://ftp.gnu.org/gnu/bison/bison-2.3.tar.gz 
tar zxvf bison-2.3.tar.gz
cd bison-2.3
./configure --prefix=${INSTALL_PATH_BISON}
make
make install

cd $ROOT
wget http://prdownloads.sourceforge.net/flex/flex-2.5.33.tar.gz
tar zxvf flex-2.5.33.tar.gz
cd flex-2.5.33
./configure --prefix=${INSTALL_PATH_FLEX}
make
make install

cd $ROOT
wget https://fossies.org/linux/misc/lm_sensors-3.4.0.tar.gz
tar zxvf lm_sensors-3.4.0.tar.gz
cd lm_sensors-3.4.0
make PREFIX=${INSTALL_PATH_SENSORS} all
make PREFIX=${INSTALL_PATH_SENSORS} install

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
rm -rf bison-2.3
rm -rf flex-2.5.33
rm -rf lm_sensors-3.4.0

# ============================================================================ #
# COMPILING                                                                    #
# ============================================================================ #

