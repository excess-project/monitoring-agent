CC = /usr/bin/gcc

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

COMMON = ../..
SRC = src
LIB = lib
TESTS = tests
UTILS = $(SRC)/utils
BINARIES = $(COMMON)/../binaries

PAPI = -L$(BINARIES)/papi/lib -lpapi
PAPI_INC = -I$(BINARIES)/papi/include/

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_papi_client mf_plugin_papi.so

mf_plugin_papi.so: mf_plugin_papi.o mf_papi_connector.o
	$(CC) -o $(LIB)/mf_plugin_papi.so $^ -shared $(BINARIES)/papi/lib/libpapi.so

mf_papi_connector.o: $(SRC)/mf_papi_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI) -lpapi

mf_plugin_papi.o: $(SRC)/mf_plugin_papi.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI) -lpapi

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
