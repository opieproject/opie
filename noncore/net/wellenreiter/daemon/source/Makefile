# $Id: Makefile,v 1.7 2002-12-27 16:57:37 mjm Exp $

CPP		= g++
CPPFLAGS	= -g -Wall  -pedantic -DDEBUG -I ../../
LIBS		= -lpcap ../../libwellenreiter/source/libwellenreiter.a
OBJ		= daemon.o

.SUFFIXES:
.PHONY:		all wellenreiterd clean distclean realclean

%.o : %.cc
	$(CPP) $(CPPFLAGS) -c $< -o $@

all:	wellenreiterd

wellenreiterd:	$(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) $(LIBS) -o $@
	@echo Build wellenreiterd

clean distclean realclean:
	@rm -f wellenreiterd *~ *.o
	@echo All dependent files have been removed.

daemon.o:	config.hh

