TEMPLATE	= app
CONFIG		+= qtopia warn_on release
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= 
SOURCES		= main.cpp
INTERFACES	= 

TARGET		= opie-update-symlinks
INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include .
LIBS            += -lqpe

include ( $(OPIEDIR)/include.pro )

