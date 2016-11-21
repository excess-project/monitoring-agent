CC = gcc

COPT_SO = $(CFLAGS) -fPIC

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(CURL_INC) $(CORE_INC) $(EXCESS_INC) $(PARSER_INC)

LFLAGS =  -lm $(PARSER) $(CURL)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

COMMON = ${CURDIR}/../..
CORE_INC = -I$(COMMON)/core
EXCESS_INC = -I$(COMMON)/agent
PARSER_PATH = $(COMMON)/contrib/parser
PARSER = -L$(COMMON)/contrib/parser/ -lparser
PARSER_INC = -I$(COMMON)/contrib/parser/src

CURL_PATH = $(COMMON)/../bin/curl/lib
CURL = -L$(COMMON)/../bin/curl/lib -lcurl
CURL_INC = -I$(COMMON)/../bin/curl/include/

all: clean-all libpublisher.so libpublisher.a

publisher.o:
	$(CC) -c src/publisher.c $(COPT_SO) $(LFLAGS)

libpublisher.so: publisher.o $(COMMON)/agent/util.o
	$(CC) -shared -o $@ $^ -lrt -ldl -Wl,-rpath,$(CURL_PATH):$(PARSER_PATH) $(CFLAGS) $(LFLAGS)

libpublisher.a:	publisher.o $(COMMON)/agent/util.o
	ar rcs $@ $^

clean:
	rm -rf *.o

clean-all:
	rm -rf *.o *.a *.so

doc: $(FILES)
	doxygen Doxyfile