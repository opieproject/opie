DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= plucker.h

SOURCES		= plucker.cpp \
                  Palm2QImage.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= Plucker
LIBS            += -lreader_pdb -lreader_pluckerbase -lreader_codec

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
