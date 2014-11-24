CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall
LFLAGS = -lm

SRC = ../src
LIBS = ../libs
INI = $(LIBS)/ini
COMMON_LIBS = ../../libs
CUTEST = $(COMMON_LIBS)/CuTest


DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

all: test_parser

test_parser:
	$(CC) -o test $(CUTEST)/*.c test_ini_parser.c $(SRC)/ini_parser.c $(INI)/ini.c $(CFLAGS)

clean:
	rm -rf test
