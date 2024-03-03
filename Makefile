CFLAGS=-g -Wall
# CFLAGS+=-DDEBUGLOG

# LUAINC?=-I/usr/local/include
LUAINC?=-I/usr/include/lua51
SHARED=--shared -fPIC
SO=so
LIBS=-lpthread

all : liblservice.$(SO)

SRCS=\
 src/service.c \
 src/queue.c \
 src/message.c

liblservice.$(SO) : $(SRCS)
	$(CC) $(CFLAGS) $(SHARED) $(LUAINC) -Isrc -o $@ $^ $(LIBS)

install:
	cp liblservice.so /usr/local/lib

test: test.c
	$(CC) $(CFLAGS) test.c -L. -llservice -Isrc -o test 

clean :
	rm -rf *.$(SO)


