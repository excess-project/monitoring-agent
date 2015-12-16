#!/bin/bash

# Build Script for Jenkins

Default_path=`pwd`

# PAPI-C
wget http://icl.cs.utk.edu/projects/papi/downloads/papi-5.4.0.tar.gz
tar zxvf papi-5.4.0.tar.gz
cd papi-5.4.0/src
./configure --prefix=`pwd`/../../binaries/papi --with-components="rapl coretemp infiniband"
make
make install all
cd $Default_path

# Likwid
wget http://ftp.fau.de/pub/likwid/likwid-3.1.3.tar.gz
tar zxvf likwid-3.1.3.tar.gz
cd likwid-3.1.3
sed -i 's/^SHARED_LIBRARY\s=\sfalse/SHARED_LIBRARY = true/' config.mk
sed -i -e 's@^PREFIX\s=\s/usr/local@PREFIX = '"$Default_path"'/binaries/likwid@' config.mk
make
sed -i 's/^SHARED_LIBRARY\s=\strue/SHARED_LIBRARY = false/' config.mk
make
make install
cd $Default_path

# CURL
wget http://curl.haxx.se/download/curl-7.37.0.tar.gz
tar zxvf curl-7.37.0.tar.gz
cd curl-7.37.0
./configure --prefix=`pwd`/../binaries/curl
make
make install
make install all
cd $Default_path

# Apache APR
wget http://mirror.23media.de/apache//apr/apr-1.5.1.tar.gz
tar zxvf apr-1.5.1.tar.gz
cd apr-1.5.1
./configure --prefix=`pwd`/../binaries/apr
make
make install
make install all
cd $Default_path

# Apache APR-Util
wget http://mirror.23media.de/apache//apr/apr-util-1.5.3.tar.gz
tar zxvf apr-util-1.5.3.tar.gz
cd apr-util-1.5.3
./configure --prefix=`pwd`/../binaries/apr --with-apr=`pwd`/../binaries/apr
make
make install
make install all
cd $Default_path

#for Nvidia GDK install
mkdir nvidia_gdk_download
cd nvidia_gdk_download
wget http://developer.download.nvidia.com/compute/cuda/7.5/Prod/gdk/gdk_linux_amd64_352_55_release.run
chmod +x gdk_linux_amd64_352_55_release.run
./gdk_linux_amd64_352_55_release.run --silent --installdir=`pwd`/../binaries/nvidia_gdk
cd $Default_path
# Clean-up
rm -f *.tar.gz
