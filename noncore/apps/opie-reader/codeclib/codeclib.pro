DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= pdb.h

SOURCES		= CExpander.cpp CBuffer.cpp StyleConsts.cpp \
		  hrule.cpp Navigation.cpp Bkmks.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/lib
TARGET		= reader_codec

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
