CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(EXCESS_INC) $(PARSER_INC) $(CORE_INC) $(COMMON_INC)

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
BINARIES = $(COMMON)/../binaries

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_iostat_client mf_plugin_iostat.so

mf_plugin_iostat.so: mf_plugin_iostat.o mf_iostat_connector.o
	$(CC) -o $(LIB)/mf_plugin_iostat.so $^ -shared

mf_iostat_connector.o: $(SRC)/mf_iostat_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_plugin_iostat.o: $(SRC)/mf_plugin_iostat.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_iostat_client: $(UTILS)/mf_iostat_client.c $(SRC)/mf_iostat_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_iostat_client
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
