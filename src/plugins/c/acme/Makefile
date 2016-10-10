## Copyright (C) 2014-2015 University of Stuttgart
## Authors: Dennis Hoppe

CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PARSER_INC) $(PUBLISHER_INC) $(EXCESS_INC) $(CORE_INC) $(COMMON_INC) $(LIBIIO_INC)

LFLAGS = $(PARSER) $(PUBLISHER) $(CURL) $(LIBIIO)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
    CFLAGS += -DNDEBUG
endif

COMMON = ../../..
SRC = src
LIB = lib
UTILS = $(SRC)/utils
BINARIES = $(COMMON)/../bin

CURL = -L$(BINARIES)/curl/lib -lcurl

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

PUBLISHER = -L$(COMMON)/contrib/publisher/ -lpublisher
PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

LIBIIO = -L$(BINARIES)/libiio/lib/ -liio
LIBIIO_INC = -I$(BINARIES)/libiio/include 

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_acme_client mf_acme_units mf_plugin_acme.so

mf_plugin_acme.so: mf_plugin_acme.o mf_acme_connector.o
	$(CC) -o $(LIB)/mf_plugin_acme.so $^ -shared $(BINARIES)/libiio/lib/x86_64-linux-gnu/libiio.so

mf_acme_connector.o: $(SRC)/mf_acme_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(LIBIIO)

mf_plugin_acme.o: $(SRC)/mf_plugin_acme.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(LIBIIO)

mf_acme_client: $(UTILS)/mf_acme_client.c $(SRC)/mf_acme_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

mf_acme_units: $(UTILS)/mf_acme_units.c $(SRC)/mf_acme_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_acme_client
	rm -f mf_acme_units
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile