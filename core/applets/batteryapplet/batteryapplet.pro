TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS	=   battery.h batterystatus.h
SOURCES	=   battery.cpp batterystatus.cpp
TARGET		= batteryapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe -lopiecore2
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
