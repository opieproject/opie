TEMPLATE	= app
CONFIG		+= qt warn_on 
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= 
SOURCES		= main.cpp
INTERFACES	= 

INCLUDEPATH	+= $(OPIEDIR)/include
LIBS       	+= -lqpe

TARGET		= hotplug-qcop
VERSION		= 0.0.0
AUTHOR		= Michael 'Mickey' Lauer <mickey@Vanille.de>

include( $(OPIEDIR)/include.pro )
