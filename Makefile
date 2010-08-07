CC=gcc

CFLAGS=-std=gnu99

LIB_CFLAGS=-c -fPIC -g -Wall
LIB_objects=hashmap.o jhash.o

%.o: %.c
	$(CC) $(CFLAGS) $(LIB_CFLAGS) $< -o $@

libname=cutils.so

.PHONY: clean
	.IGNORE: clean

all: cutils

cutils: $(LIB_objects)
	        $(CC) -shared -o $(libname) $(LIB_objects) $(LIB_statlibs)

obj_clean:
	        -rm -f $(LIB_objects)

clean: obj_clean
	        -rm -f $(libname)
