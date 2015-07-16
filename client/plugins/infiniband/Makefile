CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(infiniband_INC) $(EXCESS_INC) $(PARSER_INC) $(CORE_INC) $(COMMON_INC)

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
infiniband_INC = -I$(BINARIES)/papi/include/

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_infiniband_client mf_plugin_infiniband.so

mf_plugin_infiniband.so: mf_plugin_infiniband.o mf_infiniband_connector.o
	$(CC) -o $(LIB)/mf_plugin_infiniband.so $^ -shared $(BINARIES)/papi/lib/libpapi.so

mf_infiniband_connector.o: $(SRC)/mf_infiniband_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI) -lpapi

mf_plugin_infiniband.o: $(SRC)/mf_plugin_infiniband.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(PAPI) -lpapi

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
