## Copyright 2014 University of Stuttgart
## Author: Anthony Sulistio edited by Nico Eichhorn
## Date: April 2013

CC = /usr/bin/gcc 
## gcc-Version 4.4.7 20120313 (Red Hat 4.4.7-4) (GCC)

COPT_SO = $(CFLAGS) -fpic

PLUGIN_DIR = plugins
PLUGIN_DEST = ../binaries/plugins
CC = gcc
OUTPUT = ../binaries/mf_client

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -pthread -D_LARGEFILE64_SOURCE $(PAPI_INC) $(APR_INC) $(CURL_INC)
 
LFLAGS =  -lm $(CURL) $(PAPI) $(APR)

## List of source files

FILES  = $(shell find . -name "*.c")

HEADER  = $(shell find . -name "*.h")

## NOTE: below is used to include boost libraries
CURL = -L../binaries/curl/lib/ -lcurl
CURL_INC = -I../binaries/curl/include/

PAPI = -L../binaries/papi/lib/
PAPI_INC =  -I../binaries/papi/include/

APR_CONFIG = ../binaries/apr/bin/apr-1-config
APU_CONFIG = ../binaries/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
#APR = -L ../binaries/apr/lib/ -lapr-1 -L ../binaries/apr/lib -laprutil-1
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)
#APR_INC = -I ../binaries/apr/include/apr-1

## to use, type: make or make all
all: excess_main $(PLUGIN_DIR)/mem_info.so

lib: excess_main_lib $(PLUGIN_DIR)/mem_info.so

%.o: %.c $(HEADER)
		$(CC) -c $< $(CFLAGS) -fpic


## main part
excess_main : excess_main.o thread_handler.o util.o plugin_discover.o \
		plugin_manager.o
		$(CC) -o $(OUTPUT) $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

excess_main_lib: excess_main.o thread_handler.o util.o plugin_discover.o \
		plugin_manager.o
		$(CC) -shared -o $(OUTPUT).so $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

#
# plugins 
#

$(PLUGIN_DIR)/mem_info.so: $(PLUGIN_DIR)/mem_info.o
	$(CC) -o $(PLUGIN_DEST)/mem_info.so $^ -shared

$(PLUGIN_DIR)/mem_info.o: $(PLUGIN_DIR)/mem_info.c $(HEADER) 
	$(CC) -c $< -o $@ $(COPT_SO) -I.



clean:
	rm -rf core *.o *.a $(OUTPUT) plugins/*.o $(PLUGIN_DEST)/*.so

## to generate the javadoc files
## to use, type: make doc
doc : $(FILES)
	doxygen Doxyfile

