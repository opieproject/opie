TEMPLATE	= app
CONFIG		+= qtopia warn_on release
DESTDIR		= $(QPEDIR)/bin

HEADERS		= 
SOURCES		= main.cpp
INTERFACES	= 

unix:LIBS       += -lqpepim -lpthread

TARGET		= quicklauncher
