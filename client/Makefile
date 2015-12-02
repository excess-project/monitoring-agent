## Copyright 2014,2015 University of Stuttgart
## Authors: Anthony Sulistio, Nico Eichhorn, Dennis Hoppe

CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

REVISION = 0.1.7
INSTALL_DIR = ../mf/$(REVISION)
INSTALL_PLUGINS_DIR = $(INSTALL_DIR)/bin/plugins
INSTALL_INCLUDES_DIR = $(INSTALL_DIR)/include
INSTALL_LIB_DIR = $(INSTALL_DIR)/lib

BINARIES = ../binaries
PLUGIN_DIR = plugins
PLUGIN_DEST = $(BINARIES)/plugins
OUTPUT = $(BINARIES)/mf_agent

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -fPIC -pthread -D_LARGEFILE64_SOURCE \
$(PAPI_INC) \
$(APR_INC) \
$(CURL_INC) \
$(PARSER_INC) \
$(PUBLISHER_INC) \
$(CORE_INC)

LFLAGS =  -lm $(CURL) $(PAPI) $(APR) $(PARSER) $(PUBLISHER)

SRC = src
FILES = $(shell find $(SRC) -name "*.c")
HEADER = $(shell find $(SRC) -name "*.h")

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

CONTRIB = contrib
PARSER = -L$(CONTRIB)/parser/ -lparser
PARSER_INC = -I$(CONTRIB)/parser/src/

EXCESS_INC = -Isrc

PUBLISHER = -L$(CONTRIB)/publisher/ -lpublisher
PUBLISHER_INC = -I$(CONTRIB)/publisher/src/

MF_API_SRC = api/src
MF_API_INC = -I$(MF_API_SRC)

#
# EXTERNAL LIBS
#
CURL = -L$(BINARIES)/curl/lib/ -lcurl
CURL_INC = -I$(BINARIES)/curl/include/

PAPI = -L$(BINARIES)/papi/lib/
PAPI_INC =  -I$(BINARIES)/papi/include/

LIKWID_DIR = $(BINARIES)/likwid

APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

#
# TARGETS
#
.PHONY: libmf.so plugins copy_plugins clean

all: prepare excess_main copy_plugins

lib: libmf.so

$(SRC)/%.o: %.c $(HEADER)
		$(CC) -c $< $(CFLAGS) -fpic

prepare:
	@mkdir -p $(PLUGIN_DEST)
	$(MAKE) -C contrib/parser DEBUG=$(DEBUG)
	$(MAKE) -C contrib/publisher DEBUG=$(DEBUG)
	@mkdir -p lib/
	cp -f contrib/parser/libparser.so lib/
	cp -f contrib/publisher/libpublisher.so lib/

excess_main: $(SRC)/excess_main.o $(SRC)/thread_handler.o $(SRC)/util.o $(SRC)/plugin_discover.o $(SRC)/plugin_manager.o
	$(CC) -o $(OUTPUT) $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

mf_api.o:
	$(CC) -c $(MF_API_SRC)/mf_api.c -o $@ $(COPT_SO) -I. $(MF_API_INC) $(CFLAGS) $(EXCESS_INC) $(MF) $(LFLAGS)

libmf.so: mf_api.o $(SRC)/excess_main.o $(SRC)/thread_handler.o $(SRC)/util.o $(SRC)/plugin_discover.o $(SRC)/plugin_manager.o
	$(CC) -shared -o lib/libmf.so $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(MF) $(LFLAGS)
	rm -f mf_api.o

#
# INSTALL
install: prepare-install copy_tasks

prepare-install: clean-install
	@mkdir -p $(INSTALL_DIR)
	@mkdir -p $(INSTALL_PLUGINS_DIR)
	@mkdir -p $(INSTALL_DIR)/lib
	@mkdir -p $(INSTALL_INCLUDES_DIR)

copy_tasks: copy_ini copy_scripts copy_plugins_to_install copy_agent copy_likwid copy_libs copy_includes

copy_ini:
	cp -f ../mf_config.ini $(INSTALL_DIR)

copy_scripts:
	cp -r ../scripts $(INSTALL_DIR)
	sed -i "s/REV/${REVISION}/g" $(INSTALL_DIR)/scripts/setenv.sh

copy_plugins_to_install:
	cp -f $(PLUGIN_DIR)/papi/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/likwid/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/rapl/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/meminfo/lib/*.so $(INSTALL_PLUGINS_DIR)/
	#cp -f $(PLUGIN_DIR)/iostat/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/iostat-shell/* $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/nvidia-shell/* $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/vmstat/lib/*.so $(INSTALL_PLUGINS_DIR)/
	cp -f $(PLUGIN_DIR)/infiniband/lib/*.so $(INSTALL_PLUGINS_DIR)/

copy_includes:
	cp -f api/src/mf_api.h $(INSTALL_INCLUDES_DIR)

copy_agent:
	cp -f $(OUTPUT) $(INSTALL_DIR)/bin

copy_libs:
	cp -f $(BINARIES)/papi/lib/libpapi.so* $(INSTALL_DIR)/lib
	cp -f $(BINARIES)/papi/lib/libpfm.so* $(INSTALL_DIR)/lib
	cp -f $(BINARIES)/likwid/lib/liblikwid.so $(INSTALL_DIR)/lib
	cp -f $(BINARIES)/likwid/sbin/likwid-accessD $(INSTALL_DIR)/lib
	cp -f lib/* $(INSTALL_DIR)/lib

# work-around for EXCESS cluster
copy_likwid:
	cp -r $(LIKWID_DIR) $(INSTALL_DIR)/likwid

# libs=/home/hopped/hlrs/codebase/excess/gitlab/monitoring-framework-client/mf/0.1.3/lib

#
#
# PLUG-IN MANAGEMENT
#
plugins:
	$(MAKE) -C $(PLUGIN_DIR)/papi DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/likwid DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/rapl DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/meminfo DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/vmstat DEBUG=$(DEBUG)
	#$(MAKE) -C $(PLUGIN_DIR)/iostat DEBUG=$(DEBUG)
	$(MAKE) -C $(PLUGIN_DIR)/infiniband DEBUG=$(DEBUG)

copy_plugins: plugins
	cp -f $(PLUGIN_DIR)/papi/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/likwid/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/rapl/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/meminfo/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/vmstat/lib/*.so $(PLUGIN_DEST)/
	#cp -f $(PLUGIN_DIR)/iostat/lib/*.so $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/iostat-shell/* $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/nvidia-shell/* $(PLUGIN_DEST)/
	cp -f $(PLUGIN_DIR)/infiniband/lib/*.so $(PLUGIN_DEST)/

#
# CLEAN-UP
#
clean:
	rm -rf *.o *.a *.so $(SRC)/*.o $(OUTPUT) plugins/*.o $(PLUGIN_DEST)/*.so lib

clean-all:
	$(MAKE) -C contrib/parser clean
	$(MAKE) -C contrib/publisher clean
	$(MAKE) -C $(PLUGIN_DIR)/papi clean
	$(MAKE) -C $(PLUGIN_DIR)/likwid clean
	$(MAKE) -C $(PLUGIN_DIR)/rapl clean
	$(MAKE) -C $(PLUGIN_DIR)/meminfo clean
	$(MAKE) -C $(PLUGIN_DIR)/vmstat clean
	#$(MAKE) -C $(PLUGIN_DIR)/iostat clean
	$(MAKE) -C $(PLUGIN_DIR)/infiniband clean

clean-install:
	rm -rf $(INSTALL_DIR)

#
# JAVADOC
#
doc : $(FILES)
	doxygen Doxyfile
