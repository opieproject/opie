TEMPLATE      =  lib
CONFIG        += qt plugin warn_on 
HEADERS       =  opietoothapplet.h opietoothappletimpl.h
SOURCES       =  opietoothapplet.cpp opietoothappletimpl.cpp
TARGET        =  opietooth2applet
DESTDIR       =  $(OPIEDIR)/plugins/applets
INCLUDEPATH   += $(OPIEDIR)/include ../opietooth2
DEPENDPATH    += $(OPIEDIR)/include ../opietooth2
LIBS          += -lqpe -lopietooth2
VERSION       = 0.0.3

include ( $(OPIEDIR)/include.pro )
