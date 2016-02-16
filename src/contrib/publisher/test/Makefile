CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(CURL_INC) $(CORE_INC) $(CUTEST_INC) -I../src -DDEBUG -g
LFLAGS = -lm $(CURL)

COMMON = ../../../..
COMMON_LIBS = $(COMMON)/ext

SRC = ../src

CUTEST = $(COMMON_LIBS)/CuTest
CUTEST_INC = -I$(CUTEST)
CORE_INC = -I$(COMMON)/src/core
BINARIES = $(COMMON)/bin
CURL = -L$(BINARIES)/curl/lib/ -lcurl
CURL_INC = -I$(BINARIES)/curl/include/

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

all: test_publisher

test_publisher:
	$(CC) -o test $(CUTEST)/*.c test_publisher.c $(SRC)/publisher.c $(CFLAGS) $(LFLAGS)

clean:
	rm -rf test
