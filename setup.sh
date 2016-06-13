#!/bin/bash
#  Copyright (C) 2015, 2016 University of Stuttgart
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

PAPI="papi"
PAPI_VERSION="5.4.0"
LIKWID="likwid"
LIKWID_VERSION="3.1.3"
CURL="curl"
CURL_VERSION="7.37.0"
APR="apr"
APR_VERSION="1.5.2"
APR_UTIL="apr-util"
APR_UTIL_VERSION="1.5.4"

# ============================================================================ #
# DOWNLOAD AND INSTALL PAPI-C                                                  #
# ============================================================================ #

cd $ROOT
wget http://icl.cs.utk.edu/projects/papi/downloads/${PAPI}-${PAPI_VERSION}.tar.gz
if [ ! -f ${PAPI}-${PAPI_VERSION}.tar.gz ]; then
    echo "[ERROR] File not found: " ${PAPI}-${PAPI_VERSION}.tar.gz
    exit 1;
fi
tar zxvf ${PAPI}-${PAPI_VERSION}.tar.gz
cd ${PAPI}-${PAPI_VERSION}/src
./configure --prefix=${INSTALL_PATH_PAPI} --with-components="rapl coretemp infiniband"
make
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL LIKWID                                                  #
# ============================================================================ #

cd $ROOT
wget http://ftp.fau.de/pub/likwid/${LIKWID}-${LIKWID_VERSION}.tar.gz
if [ ! -f ${LIKWID}-${LIKWID_VERSION}.tar.gz ]; then
    echo "[ERROR] File not found: " ${LIKWID}-${LIKWID_VERSION}.tar.gz
    exit 1;
fi
tar zxvf ${LIKWID}-${LIKWID_VERSION}.tar.gz
cd ${LIKWID}-${LIKWID_VERSION}
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
wget http://curl.haxx.se/download/${CURL}-${CURL_VERSION}.tar.gz
if [ ! -f ${CURL}-${CURL_VERSION}.tar.gz ]; then
    echo "[ERROR] File not found: " ${CURL}-${CURL_VERSION}.tar.gz
    exit 1;
fi
tar zxvf ${CURL}-${CURL_VERSION}.tar.gz
cd ${CURL}-${CURL_VERSION}
./configure --prefix=${INSTALL_PATH_CURL}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL APACHE APR                                              #
# ============================================================================ #

cd $ROOT
wget http://www.eu.apache.org/dist/apr/${APR}-${APR_VERSION}.tar.gz
if [ ! -f ${APR}-${APR_VERSION}.tar.gz ]; then
    echo "[ERROR] File not found: " ${APR}-${APR_VERSION}.tar.gz
    exit 1;
fi
tar zxvf ${APR}-${APR_VERSION}.tar.gz
cd ${APR}-${APR_VERSION}
./configure --prefix=${INSTALL_PATH_APR}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL APACHE APR UTILITIES                                    #
# ============================================================================ #

cd $ROOT
wget http://www.eu.apache.org/dist//apr/apr-util-1.5.4.tar.gz
if [ ! -f ${APR_UTIL}-${APR_UTIL_VERSION}.tar.gz ]; then
    echo "[ERROR] File not found: " ${APR_UTIL}-${APR_UTIL_VERSION}.tar.gz
    exit 1;
fi
tar zxvf ${APR_UTIL}-${APR_UTIL_VERSION}.tar.gz
cd ${APR_UTIL}-${APR_UTIL_VERSION}
./configure --prefix=${INSTALL_PATH_APU} --with-apr=${INSTALL_PATH_APR}
make
make install
make install all

# ============================================================================ #
# DOWNLOAD AND INSTALL NVIDIA GDK                                              #
# ============================================================================ #

cd $ROOT
mkdir -f nvidia_gdk_download
cd nvidia_gdk_download
NVIDIA_BASE_URL="http://developer.download.nvidia.com"
NVIDIA_GDK="gdk_linux_amd64_352_55_release.run"
wget ${NVIDIA_BASE_URL}/compute/cuda/7.5/Prod/gdk/${NVIDIA_GDK}
if [ ! -f ${NVIDIA_GDK} ]; then
    echo "[ERROR] File not found: " ${NVIDIA_GDK}
    exit 1;
fi
chmod +x ${NVIDIA_GDK}
./${NVIDIA_GDK} --silent --installdir=${INSTALL_PATH_NVIDIA}

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

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${INSTALL_PATH_FLEX}/lib:${INSTALL_PATH_BISON}/lib
export PATH=${PATH}:${INSTALL_PATH_BISON}/bin:${INSTALL_PATH_FLEX}/bin

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
rm -rf ${PAPI}-${PAPI_VERSION}
rm -rf ${LIKWID}-${LIKWID_VERSION}
rm -rf ${APR}-${APR_VERSION}
rm -rf ${APR_UTIL}-${APR_UTIL_VERSION}
rm -rf ${CURL}-${CURL_VERSION}
rm -rf nvidia_gdk_download
rm -rf bison-2.3
rm -rf flex-2.5.33
rm -rf lm_sensors-3.4.0
rm -rf ${NVIDIA_GDK}

# ============================================================================ #
# DONE
# ============================================================================ #

echo "ALL DEPENDENCIES WERE INSTALLED SUCCESSFULLY!"
