TEMPLATE	= app
CONFIG		+= qtopia warn_on 
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= dropins.h
SOURCES		= main.cpp
INTERFACES	= 

INCLUDEPATH	+= $(OPIEDIR)/include
LIBS       	+= -lqpe -lopiecore2 -lopieui2

TARGET		= quicklauncher


include ( $(OPIEDIR)/include.pro )
