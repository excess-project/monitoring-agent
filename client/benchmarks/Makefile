CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(MF_API)

LFLAGS = $(MF_LIB) -Wl,-rpath-link,lib

BASE = ..
SRC = src
LIB = $(BASE)/lib
INC = $(BASE)/api/src

MF_LIB = -L$(LIB) -lmf
MF_API = -I$(INC)

.PHONY: database_access_latency

all: database_access_latency

database_access_latency:
	$(CC) $(SRC)/database_access_latency.c -o $@ $(CFLAGS) $(LFLAGS)

clean:
	rm -rf database_access_latency
