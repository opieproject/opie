TEMPLATE        = lib
CONFIG          += qte warn_on 
HEADERS 	= connection.h parser.h device.h manager.h remotedevice.h services.h startpanconnection.h startdunconnection.h 
SOURCES		= connection.cpp parser.cc device.cc manager.cc remotedevice.cc services.cc startpanconnection.cpp startdunconnection.cpp
TARGET		= opietooth1
INCLUDEPATH += $(OPIEDIR)/include .
DESTDIR		= $(OPIEDIR)/lib
LIBS += -lopiecore2

include ( $(OPIEDIR)/include.pro )
