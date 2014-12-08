CC = /usr/bin/gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(APR_INC) -I$(COMMON_INC)

LFLAGS =  -lm $(APR)

COMMON_INC = ../../
BINARIES = ../../../binaries
APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

all: clean-all libparser.a

mf_parser.o:
		$(CC) -c src/mf_parser.c libs/ini/ini.c -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libparser.a:	mf_parser.o ini.o
		ar rcs $@ $^

clean:
	rm -rf core *.o

clean-all:
	rm -rf core *.o *.a

doc: $(FILES)
	doxygen Doxyfile

