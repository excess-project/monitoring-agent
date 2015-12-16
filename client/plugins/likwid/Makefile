CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(LIKWID_INC) $(EXCESS_INC) $(PARSER_INC) $(CORE_INC) $(COMMON_INC)

LFLAGS = $(LIKWID) $(PARSER)

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

LIKWID = -L$(BINARIES)/likwid/lib/
LIKWID_INC = -I$(BINARIES)/likwid/include/likwid

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_plugin_likwid.so

mf_plugin_likwid.so: mf_plugin_likwid.o mf_likwid_connector.o
	$(CC) -o $(LIB)/mf_plugin_likwid.so $^ -shared liblikwid.so

mf_likwid_connector.o: $(SRC)/mf_likwid_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(LIKWID) -llikwid -lm -lpthread

mf_plugin_likwid.o: $(SRC)/mf_plugin_likwid.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(LIKWID) -llikwid

prepare:
	@mkdir -p lib
	cp -f $(BINARIES)/likwid/lib/liblikwid.so .

clean:
	rm -rf *.o
	rm -rf liblikwid.so
	rm -rf lib
	rm -rf likwid

doc: $(FILES)
	doxygen Doxyfile
