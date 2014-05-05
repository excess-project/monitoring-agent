## Copyright 2014 University of Stuttgart
## Author: Anthony Sulistio
## Date: April 2013

CC = gcc
OUTPUT = main
CFLAGS = -ansi -pedantic -Wall -Wwrite-strings -Wpointer-arith -Wcast-align -O3
LFLAGS = -lcurl -lm

## List of source files
<<<<<<< HEAD
#FILES = http-post.c
FILES  = $(shell find . -name "*.c")
=======
FILES = http-post.c 
#FILES  = $(shell find . -name "*.c")
>>>>>>> 7781a42c0e157266ff083285fb43d2391539bedc
HEADER  = $(shell find . -name "*.h")

## NOTE: below is used to include boost libraries
CURL = -L/usr/lib/x86_64-linux-gnu	

## to use, type: make or make all
all : $(FILES)
	$(CC) $(CURL) $(CFLAGS) -o $(OUTPUT) $(FILES) $(LFLAGS)

clean:
	rm $(OUTPUT)

## to generate the javadoc files
## to use, type: make doc
doc : $(FILES)
	doxygen Doxyfile

