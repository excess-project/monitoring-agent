## Copyright (C) 2014-2015 University of Stuttgart
## Authors: Dennis Hoppe
CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(infiniband_INC) $(EXCESS_INC) $(PARSER_INC) $(PUBLISHER_INC) $(CORE_INC) $(COMMON_INC) $(APR_INC)

LFLAGS = $(PAPI) $(PARSER) $(PUBLISHER) $(CURL) $(APR)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

COMMON = ${CURDIR}/../../..
SRC = src
LIB = lib
TESTS = tests
UTILS = $(SRC)/utils
BINARIES = $(COMMON)/../bin
APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

CURL = -L$(BINARIES)/curl/lib -lcurl
PAPI = -L$(BINARIES)/papi/lib -lpapi
infiniband_INC = -I$(BINARIES)/papi/include/

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

PUBLISHER = -L$(COMMON)/contrib/publisher/ -lpublisher
PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_infiniband_client mf_plugin_infiniband.so

mf_plugin_infiniband.so: mf_plugin_infiniband.o mf_infiniband_connector.o
	$(CC) -shared -o $(LIB)/mf_plugin_infiniband.so $^ -lrt -ldl -Wl,-rpath,$(BINARIES)/papi/lib $(PAPI)

mf_infiniband_connector.o: $(SRC)/mf_infiniband_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI)

mf_plugin_infiniband.o: $(SRC)/mf_plugin_infiniband.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI)

mf_infiniband_client: $(UTILS)/mf_infiniband_client.c $(SRC)/mf_infiniband_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_infiniband_client
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
