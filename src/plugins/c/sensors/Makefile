## Copyright (C) 2016 University of Stuttgart
## Authors: Dennis Hoppe
CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(SENSORS_INC) $(PARSER_INC) $(PUBLISHER_INC) $(EXCESS_INC) $(CORE_INC) $(COMMON_INC)

LFLAGS = $(SENSORS) $(PARSER) $(PUBLISHER) $(APR)

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

SENSORS = -L${BINARIES}/sensors/lib -lsensors
SENSORS_INC = -I${BINARIES}/sensors/include/sensors

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

PUBLISHER = -L$(COMMON)/contrib/publisher/ -lpublisher
PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

FILES = $(shell find src -name '*.c')
HEADER = $(shell find src -name '*.h')

all: clean prepare mf_sensors_client mf_sensors_units mf_plugin_sensors.so

prepare:
	@mkdir -p lib

mf_plugin_sensors.so: mf_plugin_sensors.o mf_sensors_connector.o
	$(CC) -o $(LIB)/mf_plugin_sensors.so $^ -shared ${BINARIES}/sensors/lib/libsensors.so

mf_sensors_connector.o: $(SRC)/mf_sensors_connector.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_plugin_sensors.o: $(SRC)/mf_plugin_sensors.c $(HEADER)
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_sensors_client: $(UTILS)/mf_sensors_client.c $(SRC)/mf_sensors_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

mf_sensors_units: $(UTILS)/mf_sensors_units.c $(SRC)/mf_sensors_connector.c
	$(CC) $^ -o $@ -I. -Isrc $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o
	rm -rf lib
	rm -f mf_sensors_client
	rm -f mf_sensors_units