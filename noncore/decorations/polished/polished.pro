TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS		= polished.h
SOURCES		= polished.cpp
TARGET		= polished
DESTDIR		= $(OPIEDIR)/plugins/decorations
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

