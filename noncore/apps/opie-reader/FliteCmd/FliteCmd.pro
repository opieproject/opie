DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= flitecmd.h

SOURCES		= flitecmd.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/outcodecs
TARGET		= flitecmd


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
