CC=gcc

CFLAGS=-std=gnu99 \
       -g         \
       -ggdb      \
       -fPIC      \
       -Wall      \
       -Wextra    \
       -Werror

TESTS=testhashmap.c test_tty.c
SOURCES=$(filter-out $(TESTS),$(shell ls *.c))
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
HEADERS=$(patsubst %.c,%.h,$(SOURCES))

%.o: %.c
	$(CC) -DTEST -c $(CFLAGS) $(LIB_CFLAGS) $< -o $@

libname=libcutils

all: libcutils test_tty

libcutils: $(OBJECTS)
	$(CC) -shared -o $(@).so.1 $(OBJECTS)
	-ln -sf $(@).so.1 $(@).so

test_tty: libcutils
	$(CC) $(CFLAGS) $(@).c -L. -lcutils -o $(@)

obj_clean:
	-rm -f *.o

clean: obj_clean
	-rm -f $(libname).so.1
	-rm -f $(libname).so
	-rm -f test_tty

.PHONY: all clean
