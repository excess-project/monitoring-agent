CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(APR_INC) $(CORE_INC) -I../src
LFLAGS = -lm $(APR)

SRC = ../src
LIBS = ../libs
INI = $(LIBS)/ini
COMMON_LIBS = ../../../libs
CUTEST = $(COMMON_LIBS)/CuTest

COMMON = ../../..
CORE_INC = -I$(COMMON)/core/src
BINARIES = ../../../../binaries
APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

all: test_parser

test_parser:
	$(CC) -o test $(CUTEST)/*.c test_mf_parser.c $(SRC)/mf_parser.c $(INI)/ini.c $(CFLAGS) $(LFLAGS)

clean:
	rm -rf test
