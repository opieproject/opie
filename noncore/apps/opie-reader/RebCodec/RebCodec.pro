DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= Reb.h

SOURCES		= Reb.cpp \
                  decompress.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= Reb
LIBS            += -L$(OPIEDIR)/lib -lreader_codec


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )

