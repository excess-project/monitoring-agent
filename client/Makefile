## Copyright 2014 University of Stuttgart
## Author: Anthony Sulistio edited by Nico Eichhorn
## Date: April 2013

CC = /usr/bin/gcc
## gcc-Version 4.4.7 20120313 (Red Hat 4.4.7-4) (GCC)

COPT_SO = $(CFLAGS) -fpic

PLUGIN_DIR = plugins
PLUGIN_DEST = ../binaries/plugins
OUTPUT = ../binaries/mf_agent

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -fPIC -pthread -D_LARGEFILE64_SOURCE \
$(PAPI_INC) \
$(APR_INC) \
$(CURL_INC) \
$(LIKWID_INC) \
$(PARSER_INC) \
$(PUBLISHER_INC) \
$(CORE_INC)

LFLAGS =  -lm $(CURL) $(PAPI) $(APR) $(LIKWID) $(PARSER) $(PUBLISHER)


## List of source files
FILES  = $(shell find . -name "*.c")
HEADER  = $(shell find . -name "*.h")


## Internal Libraries
CORE_INC = -Icore/src

PARSER = -Lcontrib/parser/ -lparser
PARSER_INC = -Icontrib/parser/src/

PUBLISHER = -Lcontrib/publisher/ -lpublisher
PUBLISHER_INC = -Icontrib/publisher/src/

MF_API_SRC = api/src
MF_API_INC = -I$(MF_API_SRC)

MF = -L$(COMMON) -lagent

## External Libraries
CURL = -L../binaries/curl/lib/ -lcurl
CURL_INC = -I../binaries/curl/include/

PAPI = -L../binaries/papi/lib/
PAPI_INC =  -I../binaries/papi/include/

LIKWID = -L../binaries/likwid/lib/
LIKWID_INC = -I../binaries/likwid/include/likwid

APR_CONFIG = ../binaries/apr/bin/apr-1-config
APU_CONFIG = ../binaries/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

.PHONY: libmf.so plugins copy_plugins clean

## to use, type: make or make all
all: prepare excess_main copy_plugins $(PLUGIN_DIR)/mem_info.so $(PLUGIN_DIR)/rapl.so $(PLUGIN_DIR)/likwid.so

lib: prepare libmf.so

%.o: %.c $(HEADER)
		$(CC) -c $< $(CFLAGS) -fpic

prepare:
	@mkdir -p $(PLUGIN_DEST)
	$(MAKE) -C contrib/parser
	$(MAKE) -C contrib/publisher
	@mkdir -p lib/
	cp -f contrib/parser/libparser.so lib/
	cp -f contrib/publisher/libpublisher.so lib/

## main part
excess_main: excess_main.o thread_handler.o util.o plugin_discover.o plugin_manager.o
	$(CC) -o $(OUTPUT) $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libagent.so: excess_main.o thread_handler.o util.o plugin_discover.o plugin_manager.o
	$(CC) -shared -o $@ $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

mf_api.o:
	$(CC) -c $(MF_API_SRC)/mf_api.c -o $@ $(COPT_SO) -I. $(MF_API_INC) $(CFLAGS) $(MF) $(LFLAGS)

libmf.so: mf_api.o excess_main.o thread_handler.o util.o plugin_discover.o plugin_manager.o
	$(CC) -shared -o lib/libmf.so $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(MF) $(LFLAGS)

#
# plugins
#
plugins:
	$(MAKE) -C $(PLUGIN_DIR)/papi

copy_plugins: plugins
	cp -f $(PLUGIN_DIR)/papi/lib/*.so $(PLUGIN_DEST)/

# mem_info
$(PLUGIN_DIR)/mem_info.so: $(PLUGIN_DIR)/mem_info.o
	$(CC) -o $(PLUGIN_DEST)/mem_info.so $^ -shared

$(PLUGIN_DIR)/mem_info.o: $(PLUGIN_DIR)/mem_info.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

# rapl
$(PLUGIN_DIR)/rapl.so: $(PLUGIN_DIR)/rapl_plugin.o $(PLUGIN_DIR)/rapl.o
	$(CC) -o $(PLUGIN_DEST)/rapl.so $^ -shared ../binaries/papi/lib/libpapi.so

$(PLUGIN_DIR)/rapl_plugin.o: $(PLUGIN_DIR)/rapl_plugin.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI) -lpapi

$(PLUGIN_DIR)/rapl.o: $(PLUGIN_DIR)/rapl.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI) -lpapi

# likwid
$(PLUGIN_DIR)/likwid.so: $(PLUGIN_DIR)/likwid_plugin.o $(PLUGIN_DIR)/likwid.o
	$(CC) -o $(PLUGIN_DEST)/likwid.so $^ -shared ../binaries/likwid/lib/liblikwid.so

$(PLUGIN_DIR)/likwid_plugin.o: $(PLUGIN_DIR)/likwid_plugin.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(LIKWID) -llikwid -lm -lpthread

$(PLUGIN_DIR)/likwid.o: $(PLUGIN_DIR)/likwid.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(LIKWID) -llikwid -lm -lpthread

clean:
	rm -rf *.o *.a *.so $(OUTPUT) plugins/*.o $(PLUGIN_DEST)/*.so lib
	$(MAKE) -C contrib/parser clean
	$(MAKE) -C contrib/publisher clean

## to generate the javadoc files
## to use, type: make doc
doc : $(FILES)
	doxygen Doxyfile

