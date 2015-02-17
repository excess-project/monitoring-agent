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
BINARIES = $(COMMON)/../binaries

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_plugin_meminfo.so

mf_plugin_meminfo.so: mf_plugin_meminfo.o
	$(CC) -o $(LIB)/mf_plugin_meminfo.so $^ -shared

mf_plugin_meminfo.o: $(SRC)/mf_plugin_meminfo.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
