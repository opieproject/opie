TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS		= flat.h
SOURCES		= flat.cpp
TARGET		= flat
DESTDIR		= $(OPIEDIR)/plugins/decorations
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

