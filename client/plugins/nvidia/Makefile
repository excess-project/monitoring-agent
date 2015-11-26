CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PARSER_INC) $(EXCESS_INC) $(CORE_INC) $(COMMON_INC) $(NVML_INC)

LFLAGS = $(PARSER) $(NVML)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
    CFLAGS += -DNDEBUG
endif

COMMON = ../..
SRC = src
LIB = lib

COMMON_INC = -I$(COMMON)
CORE_INC = -I$(COMMON)/core/src

EXCESS_INC = -I$(COMMON)/src

PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

# Custom installation of the NVML library.
NVML_INC = -I$(GDK_DIR)/usr/include/nvidia/gdk
NVML = -L$(GDK_DIR)/usr/src/gdk/nvml/lib/ -lnvidia-ml

all: clean prepare mf_plugin_nvidia.so

mf_plugin_nvidia.so: mf_plugin_nvidia.o
	$(CC) -o $(LIB)/mf_plugin_nvidia.so $^ $(NVML) -shared

mf_plugin_nvidia.o: $(SRC)/mf_plugin_nvidia.c
	$(CC) -c $< -o $@ $(COPT_SO) -I.

prepare:
	@mkdir -p $(LIB)

clean:
	rm -rf *.o
	rm -rf $(LIB)

