TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= connection.h parser.h device.h manager.h remotedevice.h services.h startpanconnection.h startdunconnection.h 
SOURCES		= connection.cpp parser.cc device.cc manager.cc remotedevice.cc services.cc startpanconnection.cpp startdunconnection.cpp
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include .
DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
LIBS += -lopie
#VERSION = 0.0.0

include ( $(OPIEDIR)/include.pro )
