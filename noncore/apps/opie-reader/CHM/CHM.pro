DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= CHM.h chm_lib.h lzx.h

SOURCES		= CHM.cpp chm_lib.c lzx.c


INTERFACES	=
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= CHM
LIBS            += -lreader_codec

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )