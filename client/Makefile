## Copyright 2014 University of Stuttgart
## Author: Anthony Sulistio
## Date: April 2013

CC = gcc
OUTPUT = ../../binaries/mf_client
CFLAGS = -ansi -pedantic -Wall -Wwrite-strings -Wpointer-arith -Wcast-align -O3 -I../../binaries/curl/include/ -I../../binaries/papi/include/ 
LFLAGS =  -lm -lcurl

## List of source files
#FILES = http-post.c
FILES  = $(shell find . -name "*.c")
HEADER  = $(shell find . -name "*.h")

## NOTE: below is used to include boost libraries
CURL = -L../../binaries/curl/lib/
PAPI = -L../../binaries/papi/lib/

## to use, type: make or make all
all : $(FILES)
	$(CC) $(CURL) $(PAPI) $(CFLAGS) -o $(OUTPUT) $(FILES) $(LFLAGS)

clean:
	rm $(OUTPUT)

## to generate the javadoc files
## to use, type: make doc
doc : $(FILES)
	doxygen Doxyfile

