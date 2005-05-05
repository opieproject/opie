DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= CSource.h Model.h Coder.h SubAlloc.h

SOURCES		= Model.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/support
TARGET		= pluckerdecompress

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
