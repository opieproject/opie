TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= mobilemsg.h
SOURCES		= main.cpp mobilemsg.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= mobilemsg
DESTDIR		= $(OPIEDIR)/bin
INTERFACES	= mobilemsgbase.ui

include ( $(OPIEDIR)/include.pro )
