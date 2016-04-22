CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(CURL_INC) $(CORE_INC) $(PARSER_INC)

LFLAGS =  -lm $(CURL) $(PARSER)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

COMMON = ../..
CORE_INC = -I$(COMMON)/core
PARSER_INC = -I$(COMMON)/contrib/parser/src
CURL = -L../../../bin/curl/lib -lcurl
PARSER = -L$(COMMON)/contrib/parser/ -lparser
CURL_INC = -I../../../bin/curl/include/

all: clean-all publisher

publisher:
	$(CC) -shared src/publisher.c -o libpublisher.so -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libpublisher.a:	publisher
	ar rcs $@ $^

clean:
	rm -rf *.o

clean-all:
	rm -rf *.o *.a

doc: $(FILES)
	doxygen Doxyfile