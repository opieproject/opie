TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= calibrate.h
SOURCES		= calibrate.cpp main.cpp
INCLUDEPATH	+= $(QTDIR)/include $(OPIEDIR)/include
DEPENDPATH	+= $(QTDIR)/include $(OPIEDIR)/include
TARGET		= calibrate
LIBS		+= -lqpe 

include ( $(OPIEDIR)/include.pro )
