TEMPLATE	= app
CONFIG		+= qtopia warn_on release
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= dropins.h
SOURCES		= main.cpp
INTERFACES	= 

INCLUDEPATH	+= $(OPIEDIR)/include
LIBS       	+= -lqpe -lopie

TARGET		= quicklauncher


include ( $(OPIEDIR)/include.pro )