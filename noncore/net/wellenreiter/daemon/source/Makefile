# $Id: Makefile,v 1.3 2002-11-23 20:12:57 max Exp $

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

sniffer: sniffer.o cardmode.o
	$(CPP) $(OPTIMFLAGS) $(WARNFLAGS) sniffer.o cardmode.o $(LDFLAGS) $(LIBS) -o $@
	@echo Build sniffer


clean distclean realclean:
	@rm -rf wellenreiterd *~ *.o
	@echo All dependent files have been removed.

daemon.o:	config.hh

