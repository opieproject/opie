DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= CFilter.h \
                  util.h

SOURCES		= striphtml.cpp

# copy data file to install directory
QMAKE_PRE_LINK = mkdir -p $(OPIEDIR)/plugins/reader/data && cp ../HTMLentities $(OPIEDIR)/plugins/reader/data/HTMLentities

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/filters
TARGET		= HTMLfilter

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
