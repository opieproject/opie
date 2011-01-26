TEMPLATE      =  lib
CONFIG        += qt plugin warn_on 
HEADERS       =  bluezapplet.h
SOURCES       =  bluezapplet.cpp
TARGET        =  bluetoothapplet
DESTDIR       =  $(OPIEDIR)/plugins/applets
INCLUDEPATH   += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe
VERSION       = 0.0.5

include( $(OPIEDIR)/include.pro )
