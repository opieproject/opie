# $Id: Makefile,v 1.5 2002-11-27 22:06:43 mjm Exp $

CPP	       = g++

OPTIMFLAGS	= -g
WARNFLAGS	= -Wall  -pedantic -DDEBUG
LDFLAGS		= 
LIBS		= -lpcap ../../libwellenreiter/source/libwellenreiter.a
OBJ		= daemon.o

.SUFFIXES:
.PHONY:		all wellenreiterd clean distclean realclean

%.o : %.cc
	$(CPP) $(WARNFLAGS) $(OPTIMFLAGS) -c $< -o $@

all:	wellenreiterd

wellenreiterd:	$(OBJ)
	$(CPP) $(OPTIMFLAGS) $(WARNFLAGS) $(OBJ) $(LDFLAGS) $(LIBS) -o $@
	@echo Build wellenreiterd

clean distclean realclean:
	@rm -rf wellenreiterd *~ *.o
	@echo All dependent files have been removed.

daemon.o:	config.hh

