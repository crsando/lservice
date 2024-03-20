CFLAGS=-g -Wall
# CFLAGS+=-DDEBUGLOG

# LUAINC?=-I/usr/local/include
LUAINC?=-I/usr/local/include/luajit-2.1
SHARED=--shared -fPIC
SO=so
LIBS=-lpthread -lluajit-5.1

all : liblservice.$(SO) lua-seri.so

SRCS=\
 src/service.c \
 src/queue.c \
 src/registry.c \
 src/util.c \
 src/log.c

liblservice.$(SO) : $(SRCS)
	$(CC) $(CFLAGS) $(SHARED) $(LUAINC) -Isrc -o $@ $^ $(LIBS)

install:
	cp liblservice.so /usr/local/lib
	cp lua/lservice.lua /usr/local/share/lua/5.1/

test: test.c
	$(CC) $(CFLAGS) $(LUAINC) test.c -L. -lluajit-5.1 -llservice -Isrc -o test 

test2: test2.c
	$(CC) $(CFLAGS) $(LUAINC) test2.c -L. -lluajit-5.1 -llservice -Isrc -o test2 

test_registry: test_registry.c
	$(CC) $(CFLAGS) $(LUAINC) test_registry.c -L. -lluajit-5.1 -llservice -Isrc -o test_registry

clean :
	rm -rf *.$(SO)