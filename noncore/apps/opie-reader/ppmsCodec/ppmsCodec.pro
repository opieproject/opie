DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= Aportis.h

SOURCES		= ppm_expander.cpp \
                  ppm.cpp \
                  arith_d.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= ppms
LIBS            += -lreader_codec

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
