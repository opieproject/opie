TEMPLATE	= app
DESTDIR         = $(OPIEDIR)/bin
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= ../common/ocoppacket.h ocopclient.h ocopserver.h
SOURCES		= ../common/ocoppacket.cpp main.cpp ocopserver.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
TARGET		= ocopserver

