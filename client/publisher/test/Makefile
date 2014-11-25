CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(CURL_INC)
LFLAGS = -lm $(CURL)

COMMON_LIBS = ../../libs
CUTEST = $(COMMON_LIBS)/CuTest

SRC = ../src
BINARIES = ../../../binaries
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
