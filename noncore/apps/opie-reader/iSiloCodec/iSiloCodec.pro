DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
#HEADERS		= Aportis.h

SOURCES		= iSilo.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= iSilo
LIBS            += -lreader_pdb -lreader_codec

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )

