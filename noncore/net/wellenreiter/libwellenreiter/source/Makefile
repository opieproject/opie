# $Id: Makefile,v 1.7 2003-02-12 18:53:34 mjm Exp $

OBJ = wl_proto.o wl_sock.o wl_log.o wl_conf.o cardmode.o sniff.o 
CPP = g++
CPPFLAGS = -Wall -pedantic -g -DDEBUG

%.o : %.cc
	$(CPP) $(CPPFLAGS) -c $< -o $@

static:	libwellenreiter.a
libwellenreiter.a: $(OBJ)
	ar -cr libwellenreiter.a $(OBJ)

shared:	libwellenreiter.so
libwellenreiter.so: $(OBJ)
	$(CPP) $(CPPFLAGS) -shared -o libwellenreiter.so $(OBJ)

clean:
	rm -f *.o *.a *.so *~

