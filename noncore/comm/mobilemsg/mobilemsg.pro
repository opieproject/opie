TEMPLATE	= app
#CONFIG		= qt warn_on 
CONFIG		= qt warn_on 
HEADERS		= mobilemsg.h
SOURCES		= main.cpp mobilemsg.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= mobilemsg
DESTDIR		= $(OPIEDIR)/bin
INTERFACES	= mobilemsgbase.ui

include ( $(OPIEDIR)/include.pro )
