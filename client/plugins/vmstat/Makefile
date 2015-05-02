CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(EXCESS_INC) $(CORE_INC) $(COMMON_INC) $(PARSER_INC)

LFLAGS = $(PARSER)

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
UTILS = utils
BINARIES = $(COMMON)/../binaries

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_vmstat_client mf_plugin_vmstat.so

prepare:
	@mkdir -p lib

mf_plugin_vmstat.so: mf_plugin_vmstat.o mf_vmstat_connector.o
	$(CC) -o $(LIB)/mf_plugin_vmstat.so $^ -shared

mf_vmstat_connector.o: $(SRC)/mf_vmstat_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_plugin_vmstat.o: $(SRC)/mf_plugin_vmstat.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_vmstat_client: $(UTILS)/mf_vmstat_client.c $(SRC)/mf_vmstat_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o
	rm -rf lib
	rm -f mf_vmstat_client
