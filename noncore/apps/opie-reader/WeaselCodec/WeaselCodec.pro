DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= ztxt.h

SOURCES		= ztxt.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= Weasel
LIBS            += -lreader_pdb -lreader_codec

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
