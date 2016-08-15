CC = gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(SRC_INC)

LFLAGS = $(MF_LIB) $(PARSER_LIB) $(PUBLISHER_LIB) -Wl,-rpath-link,lib

SRC=../src
SRC_INC=-I$(SRC)
MF_BASE=../../../
BASE_LIBS=$(MF_BASE)/lib
MF_LIB=-L$(BASE_LIBS) -lmf
PARSER_LIB=-L$(BASE_LIBS) -lparser
PUBLISHER_LIB=-L$(BASE_LIBS) -lpublisher

.PHONY: mf_test

all: mf_test

prepare:
	$(MAKE) -C $(MF_BASE) lib

mf_test: prepare
	$(CC) mf_test.c -o $@ $(CFLAGS) $(LFLAGS)

clean:
	rm -rf mf_test
