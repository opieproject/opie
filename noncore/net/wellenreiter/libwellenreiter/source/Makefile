# $Id: Makefile,v 1.3 2002-11-25 20:43:21 mickeyl Exp $

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
