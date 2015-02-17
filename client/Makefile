## Copyright 2014 University of Stuttgart
## Authors: Anthony Sulistio, Nico Eichhorn, Dennis Hoppe
## Last Update: 17/02/2015
CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

PLUGIN_DIR = plugins
PLUGIN_DEST = ../binaries/plugins
OUTPUT = ../binaries/mf_agent

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -fPIC -pthread -D_LARGEFILE64_SOURCE \
$(PAPI_INC) \
$(APR_INC) \
$(CURL_INC) \
$(PARSER_INC) \
$(PUBLISHER_INC) \
$(CORE_INC)

LFLAGS =  -lm $(CURL) $(PAPI) $(APR) $(PARSER) $(PUBLISHER)

FILES  = $(shell find . -name "*.c")
HEADER  = $(shell find . -name "*.h")

#
# DEBUG SWITCH
#
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

#
# INTERNAL LIBS
#
CORE_INC = -Icore/src

PARSER = -Lcontrib/parser/ -lparser
PARSER_INC = -Icontrib/parser/src/

PUBLISHER = -Lcontrib/publisher/ -lpublisher
PUBLISHER_INC = -Icontrib/publisher/src/

MF_API_SRC = api/src
MF_API_INC = -I$(MF_API_SRC)

MF = -L$(COMMON) -lagent

#
# EXTERNAL LIBS
#
CURL = -L../binaries/curl/lib/ -lcurl
CURL_INC = -I../binaries/curl/include/

PAPI = -L../binaries/papi/lib/
PAPI_INC =  -I../binaries/papi/include/

APR_CONFIG = ../binaries/apr/bin/apr-1-config
APU_CONFIG = ../binaries/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

#
# TARGETS
#
.PHONY: libmf.so plugins copy_plugins clean

all: prepare excess_main copy_plugins $(PLUGIN_DIR)/mem_info.so

lib: prepare libmf.so

%.o: %.c $(HEADER)
		$(CC) -c $< $(CFLAGS) -fpic

prepare:
	@mkdir -p $(PLUGIN_DEST)
	$(MAKE) -C contrib/parser DEBUG=$(DEBUG)
	$(MAKE) -C contrib/publisher DEBUG=$(DEBUG)
	@mkdir -p lib/
	cp -f contrib/parser/libparser.so lib/
	cp -f contrib/publisher/libpublisher.so lib/

excess_main: excess_main.o thread_handler.o util.o plugin_discover.o plugin_manager.o
	$(CC) -o $(OUTPUT) $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libagent.so: excess_main.o thread_handler.o util.o plugin_discover.o plugin_manager.o
	$(CC) -shared -o $@ $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

mf_api.o:
	$(CC) -c $(MF_API_SRC)/mf_api.c -o $@ $(COPT_SO) -I. $(MF_API_INC) $(CFLAGS) $(MF) $(LFLAGS)

libmf.so: mf_api.o excess_main.o thread_handler.o util.o plugin_discover.o plugin_manager.o
	$(CC) -shared -o lib/libmf.so $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(MF) $(LFLAGS)

#
# PLUG-IN MANAGEMENT
#
plugins:
	$(MAKE) -C $(PLUGIN_DIR)/papi DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/likwid DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/rapl DEBUG=$(DEBUG)

copy_plugins: plugins
	cp -f $(PLUGIN_DIR)/papi/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/likwid/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/rapl/lib/*.so $(PLUGIN_DEST)/

#
# TO BE REMOVED
#
$(PLUGIN_DIR)/mem_info.so: $(PLUGIN_DIR)/mem_info.o
	$(CC) -o $(PLUGIN_DEST)/mem_info.so $^ -shared

$(PLUGIN_DIR)/mem_info.o: $(PLUGIN_DIR)/mem_info.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

clean:
	rm -rf *.o *.a *.so $(OUTPUT) plugins/*.o $(PLUGIN_DEST)/*.so lib
	$(MAKE) -C contrib/parser clean
	$(MAKE) -C contrib/publisher clean

#
# JAVADOC
#
doc : $(FILES)
	doxygen Doxyfile