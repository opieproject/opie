TEMPLATE      =  lib
CONFIG        += qt plugin warn_on 
HEADERS       =  bluezapplet.h
SOURCES       =  bluezapplet.cpp
TARGET        =  bluetoothapplet
DESTDIR       =  $(OPIEDIR)/plugins/applets
INCLUDEPATH   += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopietooth1
VERSION       = 0.0.4

include( $(OPIEDIR)/include.pro )
