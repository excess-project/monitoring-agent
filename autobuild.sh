#!/bin/bash
#This file is used for building all needed libraries in jenkins
Default_path=`pwd`

# Downloading and building papi
wget http://icl.cs.utk.edu/projects/papi/downloads/papi-5.4.0.tar.gz
tar zxvf papi-5.4.0.tar.gz
cd papi-5.4.0/src
./configure --prefix=`pwd`/../../binaries/papi --with-components="rapl coretemp"
make
make install all
cd $Default_path

# Downloading and build likwid (build both *.a and *.so)
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

# Downloading and building curl
wget http://curl.haxx.se/download/curl-7.37.0.tar.gz
tar zxvf curl-7.37.0.tar.gz
cd curl-7.37.0
./configure --prefix=`pwd`/../binaries/curl
make 
make install
make install all
cd $Default_path

# Downloading and building apr
wget http://mirror.23media.de/apache//apr/apr-1.5.1.tar.gz
tar zxvf apr-1.5.1.tar.gz
cd apr-1.5.1
./configure --prefix=`pwd`/../binaries/apr
make 
make install
make install all  
cd $Default_path

#Downloading and building apr-util
wget http://mirror.23media.de/apache//apr/apr-util-1.5.3.tar.gz
tar zxvf apr-util-1.5.3.tar.gz
cd apr-util-1.5.3
./configure --prefix=`pwd`/../binaries/apr --with-apr=`pwd`/../binaries/apr
make
make install
make install all
cd $Default_path

# Clean up downloaded files
rm -f *.tar.gz
