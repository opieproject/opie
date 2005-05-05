DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= arrierego.h

SOURCES		= arrierego.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= ArriereGo
LIBS += -L$(OPIEDIR)/lib -lreader_pdb -lreader_pluckerbase -lreader_codec

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include
DEFINES += USENEF
