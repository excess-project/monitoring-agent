## Copyright (C) 2014-2015 University of Stuttgart
## Authors: Dennis Hoppe

CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PAPI_INC) $(PARSER_INC) $(PUBLISHER_INC) $(EXCESS_INC) $(CORE_INC) $(COMMON_INC) $(APR_INC)

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
PAPI_INC = -I$(BINARIES)/papi/include/

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

PUBLISHER = -L$(COMMON)/contrib/publisher/ -lpublisher
PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_rapl_client mf_rapl_units mf_plugin_rapl.so

mf_plugin_rapl.so: mf_plugin_rapl.o mf_rapl_connector.o
	$(CC) -shared -o $(LIB)/mf_plugin_rapl.so $^ -lrt -ldl -Wl,-rpath,$(BINARIES)/papi/lib $(PAPI)

mf_rapl_connector.o: $(SRC)/mf_rapl_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI)

mf_plugin_rapl.o: $(SRC)/mf_plugin_rapl.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI)

mf_rapl_client: $(UTILS)/mf_rapl_client.c $(SRC)/mf_rapl_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

mf_rapl_units: $(UTILS)/mf_rapl_units.c $(SRC)/mf_rapl_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_rapl_client
	rm -f mf_rapl_units
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
