# $Id: Makefile,v 1.1 2002-11-22 23:39:26 mjm Exp $

INCLUDES = 
LIBRARIES =
LIBOBJ = sock.o log.o
CXX = g++ -Wall -pedantic -g  $(INCLUDES) -DDEBUG

static:	libwellenreiter.a
libwellenreiter.a: $(LIBOBJ)
	ar -cr libwellenreiter.a $(LIBOBJ)

shared:	libwellenreiter.so
libwellenreiter.so: $(LIBOBJ)
	$(CXX) -shared -o libwellenreiter.so $(LIBOBJ)

clean:
	rm -f *.o *.a *.so *~
