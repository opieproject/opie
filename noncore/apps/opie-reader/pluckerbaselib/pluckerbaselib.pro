DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= pdb.h \
                  util.h

SOURCES		= plucker_base.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/lib
TARGET		= reader_pluckerbase
LIBS		+= -ldl

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
