TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS		= fresh.h
SOURCES		= fresh.cpp
TARGET		= freshstyle
DESTDIR		= $(OPIEDIR)/plugins/styles
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0




include ( $(OPIEDIR)/include.pro )
