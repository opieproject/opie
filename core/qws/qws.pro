TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= \
		  transferserver.h \
		  qcopbridge.h \
		  oqwsserver.h
SOURCES		= \
		  main.cpp \
		  transferserver.cpp \
		  qcopbridge.cpp \
		  oqwsserver.cpp

INCLUDEPATH	+= $(OPIEDIR)/include 
DEPENDPATH	+= $(OPIEDIR)/include .
TARGET		= qws
LIBS		+= -lqpe -lopiecore2 -lopieui2

include ( $(OPIEDIR)/include.pro )
