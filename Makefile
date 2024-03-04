CFLAGS=-g -Wall
# CFLAGS+=-DDEBUGLOG

# LUAINC?=-I/usr/local/include
LUAINC?=-I/usr/local/include/luajit-2.1
SHARED=--shared -fPIC
SO=so
LIBS=-lpthread -lluajit-5.1

all : liblservice.$(SO)

SRCS=\
 src/service.c \
 src/routine.c \
 src/queue.c

liblservice.$(SO) : $(SRCS)
	$(CC) $(CFLAGS) $(SHARED) $(LUAINC) -Isrc -o $@ $^ $(LIBS)

install:
	cp liblservice.so /usr/local/lib

test: test.c
	$(CC) $(CFLAGS) test.c -L. -llservice -Isrc -o test 

test2: test2.c
	$(CC) $(CFLAGS) $(LUAINC) test2.c -L. -lluajit-5.1 -llservice -Isrc -o test2 

clean :
	rm -rf *.$(SO)


