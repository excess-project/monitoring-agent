

CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall -O0 -ggdb -I../excess/client/ $(APR_INC)

LFLAGS = -L../excess/client/ -lagent $(APR)

HEADER = $(shell find . -name "*.h")

APR_CONFIG = ../excess/binaries/apr/bin/apr-1-config
APU_CONFIG = ../excess/binaries/apr/bin/apu-1-config

APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)


all: main

%.o: %.c $(HEADER)
		$(CC) -c $< $(CFLAGS) -fpic

main: main.o
	$(CC) -o $@ $^ $(LFLAGS)



clean: 
	rm -rf core *.o *.a 