CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -fPIC \
$(SRC_INC) \
$(CORE_INC) \
$(COMMON_INC) \
$(APR_INC) \
$(MF_PARSER_INC) \
$(MF_PUBLISHER_INC)

LFLAGS = -lm $(MF) $(MF_PARSER) $(MF_PUBLISHER)

SRC = ../src
SRC_INC = -I$(SRC)

COMMON = ../..
BINARIES = $(COMMON)/../binaries

MF = -L$(COMMON) -lmf
CORE_INC = -I$(COMMON)/core/src
COMMON_INC = -I$(COMMON)

MF_PARSER = -L$(COMMON)/contrib/parser -lparser
MF_PARSER_INC = -I$(COMMON)/contrib/parser/src/

MF_PUBLISHER = -L$(COMMON)/contrib/publisher -lpublisher
MF_PUBLISHER_INC = -I$(COMMON)/contrib/publisher/src

APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

.PHONY: prepare client

all: prepare client

prepare:
	$(MAKE) -C $(COMMON) libmf

client:
	$(CC) client.c $(SRC)/mf_api.c -o client -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

clean:
	rm -rf client
