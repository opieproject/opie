DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= pdb.h

SOURCES		= pdb.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/lib
TARGET		= reader_pdb

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
