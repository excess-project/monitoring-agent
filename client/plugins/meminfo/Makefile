CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PARSER_INC) $(EXCESS_INC) $(CORE_INC) $(COMMON_INC)

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
UTILS = $(SRC)/utils
BINARIES = $(COMMON)/../bin

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_meminfo_client mf_plugin_meminfo.so

prepare:
	@mkdir -p lib

mf_plugin_meminfo.so: mf_plugin_meminfo.o mf_meminfo_connector.o
	$(CC) -o $(LIB)/mf_plugin_meminfo.so $^ -shared

mf_meminfo_connector.o: $(SRC)/mf_meminfo_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_plugin_meminfo.o: $(SRC)/mf_plugin_meminfo.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_meminfo_client: $(UTILS)/mf_meminfo_client.c $(SRC)/mf_meminfo_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o
	rm -rf lib
	rm -f mf_meminfo_client