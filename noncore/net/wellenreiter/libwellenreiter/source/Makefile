# $Id: Makefile,v 1.2 2002-11-24 21:20:00 mjm Exp $

INCLUDES = 
LIBRARIES =
LIBOBJ = proto.o sock.o log.o
CXX = g++ -Wall -pedantic -g  $(INCLUDES) -DDEBUG

static:	libwellenreiter.a
libwellenreiter.a: $(LIBOBJ)
	ar -cr libwellenreiter.a $(LIBOBJ)

shared:	libwellenreiter.so
libwellenreiter.so: $(LIBOBJ)
	$(CXX) -shared -o libwellenreiter.so $(LIBOBJ)

clean:
	rm -f *.o *.a *.so *~
