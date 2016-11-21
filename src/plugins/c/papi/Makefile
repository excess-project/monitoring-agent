## Copyright (C) 2014-2015 University of Stuttgart
## Authors: Dennis Hoppe

CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PAPI_INC) $(EXCESS_INC) $(PARSER_INC) $(CORE_INC) $(COMMON_INC)

LFLAGS = $(PAPI) $(PARSER)

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

PAPI = -L$(BINARIES)/papi/lib -lpapi
PAPI_INC = -I$(BINARIES)/papi/include/

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_papi_client mf_plugin_papi.so

mf_plugin_papi.so: mf_plugin_papi.o mf_papi_connector.o
	$(CC) -shared -o $(LIB)/mf_plugin_papi.so $^ -lrt -ldl -Wl,-rpath,$(BINARIES)/papi/lib $(PAPI)

mf_papi_connector.o: $(SRC)/mf_papi_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI)

mf_plugin_papi.o: $(SRC)/mf_plugin_papi.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI)

mf_papi_client: $(UTILS)/mf_papi_client.c $(SRC)/mf_papi_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_papi_client
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
