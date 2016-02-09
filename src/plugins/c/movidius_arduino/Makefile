## Copyright (C) 2014-2015 University of Stuttgart
## Authors: Dennis Hoppe

CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(MOVIDIUS_ARDUINO_INC) $(EXCESS_INC) $(PARSER_INC) $(CORE_INC) $(COMMON_INC)

LFLAGS = $(MOVIDIUS_ARDUINO) $(PARSER)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

COMMON = ../../..
SRC = src
LIB = lib
TESTS = tests
UTILS = $(SRC)/utils
BINARIES = $(COMMON)/../bin

MOVIDIUS_ARDUINO = -L$(BINARIES)/movidius_arduino/lib 
MOVIDIUS_ARDUINO_INC = -I$(BINARIES)/movidius_arduino/include/

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_movidius_arduino_client mf_plugin_movidius_arduino.so

mf_plugin_movidius_arduino.so: mf_plugin_movidius_arduino.o mf_movidius_arduino_connector.o
	$(CC) -o $(LIB)/mf_plugin_movidius_arduino.so $^ -shared 
	cp $(LIB)/mf_plugin_movidius_arduino.so $(BINARIES)/movidius_arduino/lib/libmovidius_arduino.so 

mf_movidius_arduino_connector.o: $(SRC)/mf_movidius_arduino_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(MOVIDIUS_ARDUINO) -lmovidius_arduino

mf_plugin_movidius_arduino.o: $(SRC)/mf_plugin_movidius_arduino.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I. $(MOVIDIUS_ARDUINO) -lmovidius_arduino

mf_movidius_arduino_client: $(UTILS)/mf_movidius_arduino_client.c $(SRC)/mf_movidius_arduino_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p lib

clean:
	rm -rf *.o
	rm -f mf_movidius_arduino_client
	rm -rf lib

doc: $(FILES)
	doxygen Doxyfile
