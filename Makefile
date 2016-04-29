## Copyright 2014,2015 University of Stuttgart
## Authors: Anthony Sulistio, Nico Eichhorn, Dennis Hoppe

CC = /usr/bin/gcc
COPT_SO = $(CFLAGS) -fpic

REVISION = 16.2
HOST=$(shell hostname)
INSTALL_DIR = dist

ifneq (,$(findstring excess,$(HOST)))
	INSTALL_DIR = /opt/mf/${REVISION}
endif

ifneq (,$(findstring jenkins,$(HOST)))
	INSTALL_DIR = dist
endif

INSTALL_PLUGINS_DIR = $(INSTALL_DIR)/bin/plugins
INSTALL_INCLUDES_DIR = $(INSTALL_DIR)/include
INSTALL_LIB_DIR = $(INSTALL_DIR)/lib

BINARIES = bin
BUILD_DIR = build
PLUGIN_DIR = $(BASE)/plugins/c
PLUGIN_DEST = $(BUILD_DIR)/plugins
OUTPUT = $(BUILD_DIR)/mf_agent

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -fPIC -pthread -D_LARGEFILE64_SOURCE \
$(PAPI_INC) \
$(APR_INC) \
$(CURL_INC) \
$(PARSER_INC) \
$(PUBLISHER_INC) \
$(CORE_INC)

LFLAGS =  -lm $(CURL) $(PAPI) $(APR) $(PARSER) $(PUBLISHER)

SRC = $(BASE)/agent
FILES = $(shell find $(SRC) -name "*.c")
HEADER = $(shell find $(SRC) -name "*.h")

#
# DEBUG SWITCH
#
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

#
# INTERNAL LIBS
#
BASE = src
CORE_INC = -I$(BASE)/core

CONTRIB = $(BASE)/contrib
PARSER = -L$(CONTRIB)/parser/ -lparser
PARSER_INC = -I$(CONTRIB)/parser/src/

EXCESS_INC = -I$(BASE)/agent

PUBLISHER = -L$(CONTRIB)/publisher/ -lpublisher
PUBLISHER_INC = -I$(CONTRIB)/publisher/src/

MF_API_SRC = $(BASE)/api/src
MF_API_INC = -I$(MF_API_SRC)

#
# EXTERNAL LIBS
#
CURL = -L$(BINARIES)/curl/lib/ -lcurl
CURL_INC = -I$(BINARIES)/curl/include/

PAPI = -L$(BINARIES)/papi/lib/
PAPI_INC =  -I$(BINARIES)/papi/include/

APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

#
# TARGETS
#
.PHONY: libmf.so plugins copy_plugins clean

all: prepare excess_main copy_plugins lib

lib: libmf.so libmf.a

$(SRC)/%.o: %.c $(HEADER)
	$(CC) -c $< $(CFLAGS) -fpic

prepare:
	@mkdir -p $(PLUGIN_DEST)
	$(MAKE) -C $(BASE)/contrib/parser DEBUG=$(DEBUG)
	$(MAKE) -C $(BASE)/contrib/publisher DEBUG=$(DEBUG)
	@mkdir -p lib
	cp -f $(BASE)/contrib/parser/libparser.so lib
	cp -f $(BASE)/contrib/publisher/libpublisher.so lib

excess_main: $(SRC)/excess_main.o $(SRC)/thread_handler.o $(SRC)/util.o $(SRC)/plugin_discover.o $(SRC)/plugin_manager.o
	$(CC) -o $(OUTPUT) $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)
	echo $(HOST)
	echo $(INSTALL_DIR)

mf_api.o:
	$(CC) -c $(MF_API_SRC)/mf_api.c -o $@ $(COPT_SO) -I. $(MF_API_INC) $(CFLAGS) $(EXCESS_INC) $(MF) $(LFLAGS)

libmf.so: mf_api.o $(SRC)/excess_main.o $(SRC)/thread_handler.o $(SRC)/util.o $(SRC)/plugin_discover.o $(SRC)/plugin_manager.o
	$(CC) -shared -o lib/libmf.so $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(MF) $(LFLAGS)
	rm -f mf_api.o

mf_api_static.o:
	$(CC) -static -c $(MF_API_SRC)/mf_api.c -o $@ -I. $(MF_API_INC) $(CFLAGS) $(EXCESS_INC) -lrt -ldl $(LFLAGS)

libmf.a: mf_api_static.o $(SRC)/excess_main.o $(SRC)/thread_handler.o $(SRC)/util.o $(SRC)/plugin_discover.o $(SRC)/plugin_manager.o
	ar rcs lib/libmf.a $^
	rm -f mf_api_static.o

#
# INSTALL
#
install: prepare-install copy_tasks

prepare-install: clean-install
	@mkdir -p $(INSTALL_DIR)
	@mkdir -p $(INSTALL_PLUGINS_DIR)
	@mkdir -p $(INSTALL_DIR)/lib
	@mkdir -p $(INSTALL_INCLUDES_DIR)
	@mkdir -p $(INSTALL_DIR)/bin/log
	echo $(REVISION) > $(INSTALL_DIR)/VERSION
	cp -f LICENSE $(INSTALL_DIR)
	cp -f NOTICE $(INSTALL_DIR)
	cp -f CHANGES $(INSTALL_DIR)

copy_tasks: copy_ini copy_scripts copy_plugins_to_install copy_agent copy_libs copy_includes

copy_ini:
	cp -f $(BASE)/mf_config.ini $(INSTALL_DIR)

copy_scripts:
	cp -r scripts $(INSTALL_DIR)
	mv $(INSTALL_DIR)/scripts/start.sh $(INSTALL_DIR)
	mv $(INSTALL_DIR)/scripts/stop.sh $(INSTALL_DIR)

copy_plugins_to_install:
	cp -f $(PLUGIN_DIR)/papi/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/movidius_arduino/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/rapl/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/meminfo/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/vmstat/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/infiniband/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/nvidia/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/sensors/lib/*.so $(INSTALL_PLUGINS_DIR)/

copy_includes:
	cp -f $(BASE)/api/src/mf_api.h $(INSTALL_INCLUDES_DIR)

copy_agent:
	cp -f $(OUTPUT) $(INSTALL_DIR)/bin

copy_libs:
	cp -f $(BINARIES)/papi/lib/libpapi.so* $(INSTALL_DIR)/lib
	cp -f $(BINARIES)/papi/lib/libpfm.so* $(INSTALL_DIR)/lib
	cp -f lib/*.so $(INSTALL_DIR)/lib
	cp -f lib/*.a $(INSTALL_DIR)/lib

#
#
# PLUG-IN MANAGEMENT
#
plugins:
	$(MAKE) -C $(PLUGIN_DIR)/papi DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/movidius_arduino DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/rapl DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/meminfo DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/vmstat DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/infiniband DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/nvidia DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/sensors DEBUG=$(DEBUG)

copy_plugins: plugins
	cp -f $(PLUGIN_DIR)/papi/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/movidius_arduino/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/rapl/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/meminfo/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/vmstat/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/infiniband/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/nvidia/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/sensors/lib/*.so $(PLUGIN_DEST)/

#
# CLEAN-UP
#
clean:
	rm -rf *.o *.a *.so $(SRC)/*.o $(OUTPUT) $(BASE)/plugins/*.o $(PLUGIN_DEST)/*.so lib build

clean-all: clean clean-install
	$(MAKE) -C $(BASE)/contrib/parser clean
	$(MAKE) -C $(BASE)/contrib/publisher clean
	$(MAKE) -C $(PLUGIN_DIR)/papi clean
	$(MAKE) -C $(PLUGIN_DIR)/movidius_arduino clean
	$(MAKE) -C $(PLUGIN_DIR)/rapl clean
	$(MAKE) -C $(PLUGIN_DIR)/meminfo clean
	$(MAKE) -C $(PLUGIN_DIR)/vmstat clean
	$(MAKE) -C $(PLUGIN_DIR)/infiniband clean
	$(MAKE) -C $(PLUGIN_DIR)/nvidia clean
	$(MAKE) -C $(PLUGIN_DIR)/sensors clean
	rm -rf bin

clean-install:
	rm -rf $(INSTALL_DIR)

#
# Generate the documentation of the monitoring client through Doxygen
#
doc:
	doxygen Doxyfile
