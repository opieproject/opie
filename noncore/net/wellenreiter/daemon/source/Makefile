# $Id: Makefile,v 1.1 2002-11-12 22:12:33 mjm Exp $

CPP	       = g++

CFLAGS	=
OPTIMFLAGS	= -g
WARNFLAGS	= -Wall  -pedantic -DDEBUG
LDFLAGS		= 
LIBS		= -lpcap
OBJ		= daemon.o log.o sendgui.o getgui.o sniffer.o

.SUFFIXES:
.PHONY:		all wellenreiterd clean distclean realclean

%.o : %.cc
	$(CPP) $(CFLAGS) $(WARNFLAGS) $(OPTIMFLAGS) -c $< -o $@

all:	wellenreiterd

wellenreiterd:	$(OBJ)
	$(CPP) $(OPTIMFLAGS) $(WARNFLAGS) $(CFLAGS) $(OBJ) $(LDFLAGS) $(LIBS) -o $@
	@echo Build wellenreiterd

clean distclean realclean:
	@rm -rf wellenreiterd *~ *.o
	@echo All dependent files have been removed.

wellenreiterd.o:	config.hh
serve.o:		config.hh
log.o:			config.hh
sendgui.o:		config.hh
