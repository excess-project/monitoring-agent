## Copyright 2014 University of Stuttgart
## Author: Anthony Sulistio edited by Nico Eichhorn
## Date: April 2013

CC = /usr/bin/gcc 
## gcc-Version 4.4.7 20120313 (Red Hat 4.4.7-4) (GCC)

COPT_SO = $(CFLAGS) -fpic

PLUGIN_DIR = plugins
CC = gcc
OUTPUT = ../binaries/mf_client
CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith -Wcast-align -ggdb -I../binaries/curl/include/ -I../binaries/papi/include/ -I../binaries/apr/include 
LFLAGS =  -lm -lcurl

## List of source files

FILES  = $(shell find . -name "*.c")

HEADER  = $(shell find . -name "*.h")

## NOTE: below is used to include boost libraries
CURL = -L../binaries/curl/lib/
PAPI = -L../binaries/papi/lib/

APR = -L../binaries/apr/lib

#PLUGIN_FILES = $(shell find ../plugin_management -name "*.c")
#PLUGIN_H = $(shell find ../plugin_management -name "*.h")

## to use, type: make or make all
all: excess_main $(PLUGIN_DIR)/mem_info.so

%.o: %.c $(HEADER)
		$(CC) -c $< $(CFLAGS) $(LFLAGS)


## main part
excess_main : excess_main.o thread_handler.o util.o plugin_discover.o \
		plugin_manager.o
		$(CC) -o $@ $^ -ldl -Wl,--export-dynamic


#
# plugins 
#

$(PLUGIN_DIR)/mem_info.so: $(PLUGIN_DIR)/mem_info.o
	$(CC) -o $@ $^ -shared

$(PLUGIN_DIR)/mem_info.o: $(PLUGIN_DIR)/mem_info.c $(HEADER) 
	$(CC) -c $< -o $@ $(COPT_SO) -I.



clean:
	rm -rf core *.o *.a excess_main plugins/*.o plugins/*.so

## to generate the javadoc files
## to use, type: make doc
doc : $(FILES)
	doxygen Doxyfile

