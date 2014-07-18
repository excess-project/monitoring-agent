## Copyright 2014 University of Stuttgart
## Author: Anthony Sulistio edited by Nico Eichhorn
## Date: April 2013

CC = /usr/bin/gcc 
## gcc-Version 4.4.7 20120313 (Red Hat 4.4.7-4) (GCC)

OUTPUT = main
CFLAGS = -I/mnt/raid/home/hpcneich/papi/include -std=gnu99 -pedantic -ggdb -Wall -Wwrite-strings -Wpointer-arith -Wcast-align 
LFLAGS = /mnt/raid/home/hpcneich/papi/lib/libpapi.a -lcurl -lm -lpthread -laprutil-1 -lapr-1
COPT_SO = $(CFLAGS) -fpic

PLUGIN_DIR = plugins

## List of source files

FILES  = $(shell find . -name "*.c")

HEADER  = $(shell find . -name "*.h")

## NOTE: below is used to include boost libraries
CURL = -L/usr/lib/x86_64-linux-gnu	

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

