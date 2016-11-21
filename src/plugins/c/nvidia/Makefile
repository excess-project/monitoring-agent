## Copyright (C) 2015 Chalmers University of Technology, Sweden
## Authors: Anders Gidenstam

CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PARSER_INC) $(PUBLISHER_INC) $(EXCESS_INC) $(CORE_INC) $(COMMON_INC) $(NVML_INC) $(APR_INC)

LFLAGS = $(PARSER) $(PUBLISHER) $(NVML) $(CURL) $(APR)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
    CFLAGS += -DNDEBUG
endif

COMMON = ${CURDIR}/../../..
SRC = src
LIB = lib
UTILS = $(SRC)/utils
BINARIES = $(COMMON)/../bin
APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

CURL = -L$(COMMON)/../bin/curl/lib -lcurl
COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core

EXCESS_INC = -I$(COMMON)/agent

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

PUBLISHER = -L$(COMMON)/contrib/publisher/ -lpublisher
PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

# Custom installation of the NVML library.
GDK_DIR = $(COMMON)/../bin/nvidia
NVML_INC = -I$(GDK_DIR)/usr/include/nvidia/gdk
NVML = -L$(GDK_DIR)/usr/src/gdk/nvml/lib/ -lnvidia-ml

all: clean prepare mf_plugin_nvidia.so mf_nvidia_units

mf_plugin_nvidia.so: mf_plugin_nvidia.o
	$(CC) -shared -o $(LIB)/mf_plugin_nvidia.so $^ -lrt -ldl -Wl,-rpath,$(GDK_DIR)/usr/src/gdk/nvml/lib $(NVML)

mf_plugin_nvidia.o: $(SRC)/mf_plugin_nvidia.c
	$(CC) -c $< -o $@ $(COPT_SO) -I.

mf_nvidia_units: $(UTILS)/mf_nvidia_units.c
	$(CC) $^ -o $@ -I. $(CFLAGS) $(LFLAGS)

prepare:
	@mkdir -p $(LIB)

clean:
	rm -rf *.o
	rm -rf mf_nvidia_units
	rm -rf $(LIB)

