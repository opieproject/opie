# $Id: Makefile,v 1.6 2002-12-08 18:48:34 mickeyl Exp $

CPP	       = g++

OPTIMFLAGS	= -g
WARNFLAGS	= -Wall  -pedantic -DDEBUG
LDFLAGS		= 
LIBS		= -lpcap ../../libwellenreiter/source/libwellenreiter.a
OBJ		= daemon.o

.SUFFIXES:
.PHONY:		all clean distclean realclean

%.o : %.cc
	$(CPP) $(WARNFLAGS) $(OPTIMFLAGS) -c $< -o $@

all:	wellenreiterd

wellenreiterd:	$(OBJ)
	$(CPP) $(OPTIMFLAGS) $(WARNFLAGS) $(OBJ) $(LDFLAGS) $(LIBS) -o $@
	@echo Built wellenreiterd

clean distclean realclean:
	@rm -rf wellenreiterd *~ *.o
	@echo All dependent files have been removed.

daemon.o:	config.hh daemon.cc daemon.hh
