TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   battery.h batterystatus.h batteryappletimpl.h
SOURCES	=   battery.cpp batterystatus.cpp batteryappletimpl.cpp
TARGET		= batteryapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe -lopie
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
