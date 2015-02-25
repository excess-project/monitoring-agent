CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(INC) $(CORE_INC)

LFLAGS = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) -Wl,-rpath-link,lib

BASE=.
INC=-I$(BASE)/../src
CORE=$(BASE)/../..
CORE_INC=-I$(CORE)/src
CORE_LIBS=$(CORE)/lib
MF_LIB=-L$(CORE_LIBS) -lmf
PARSER_LIB=-L$(CORE_LIBS) -lparser
PUBLISHER_LIB=-L$(CORE_LIBS) -lpublisher

.PHONY: mf_test

all: mf_test

prepare:
	$(MAKE) -C $(BASE)/../.. lib

mf_test: prepare
	$(CC) mf_test.c -o $@ $(CFLAGS) $(LFLAGS)

clean:
	rm -rf mf_test
