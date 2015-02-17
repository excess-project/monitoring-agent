CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(MF_INC)

LFLAGS = $(MF_LIB) -Wl,-rpath-link,lib

BASE = .
SRC = $(BASE)
LIB = $(BASE)/lib
INC = $(BASE)/include

MF_LIB = -L$(LIB) -lmf
MF_INC = -I$(INC)

.PHONY: mf_test

all: mf_test

mf_test:
	$(CC) mf_test.c -o $@ $(CFLAGS) $(LFLAGS)

clean:
	rm -rf mf_test
