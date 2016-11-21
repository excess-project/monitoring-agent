## Copyright (C) 2014-2015 University of Stuttgart
## Authors: Dennis Hoppe

CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(MOVIDIUS_ARDUINO_INC) $(EXCESS_INC) $(PARSER_INC) $(PUBLISHER_INC) $(CORE_INC) $(COMMON_INC) $(APR_INC)

LFLAGS = $(PARSER) $(PUBLISHER) $(CURL) $(APR)

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

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

PUBLISHER = -L$(COMMON)/contrib/publisher/ -lpublisher
PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_movidius_arduino_client mf_movidius_arduino_units mf_plugin_movidius_arduino.so

mf_plugin_movidius_arduino.so: mf_plugin_movidius_arduino.o mf_movidius_arduino_connector.o
	$(CC) -o $(LIB)/mf_plugin_movidius_arduino.so $^ -shared

mf_movidius_arduino_connector.o: $(SRC)/mf_movidius_arduino_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. 

mf_plugin_movidius_arduino.o: $(SRC)/mf_plugin_movidius_arduino.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_movidius_arduino_client: $(UTILS)/mf_movidius_arduino_client.c $(SRC)/mf_movidius_arduino_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

mf_movidius_arduino_units: $(UTILS)/mf_movidius_arduino_units.c $(SRC)/mf_movidius_arduino_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_movidius_arduino_client
	rm -f mf_movidius_arduino_units
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
