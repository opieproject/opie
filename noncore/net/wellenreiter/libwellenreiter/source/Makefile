# $Id: Makefile,v 1.4 2002-11-27 22:34:42 mjm Exp $

INCLUDES = 
LIBRARIES =
LIBOBJ = proto.o sock.o log.o cardmode.o sniff.o
CXX = g++ -Wall -pedantic -g  $(INCLUDES) -DDEBUG

static:	libwellenreiter.a
libwellenreiter.a: $(LIBOBJ)
	ar -cr libwellenreiter.a $(LIBOBJ)

shared:	libwellenreiter.so
libwellenreiter.so: $(LIBOBJ)
	$(CXX) -shared -o libwellenreiter.so $(LIBOBJ)

clean:
	rm -f *.o *.a *.so *~
