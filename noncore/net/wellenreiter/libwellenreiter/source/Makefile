# $Id: Makefile,v 1.5 2002-12-27 16:35:28 mjm Exp $

OBJ = wl_proto.o wl_sock.o wl_log.o cardmode.o sniff.o
CPP = g++
CPPFLAGS = -Wall -pedantic -g -DDEBUG

static:	libwellenreiter.a
libwellenreiter.a: $(OBJ)
	ar -cr libwellenreiter.a $(OBJ)

shared:	libwellenreiter.so
libwellenreiter.so: $(OBJ)
	$(CPP) $(CPPFLAGS) -shared -o libwellenreiter.so $(OBJ)

clean:
	rm -f *.o *.a *.so *~

