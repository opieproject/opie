TEMPLATE      =  lib
CONFIG        += qt warn_on release
HEADERS       =  bluezapplet.h bluezappletimpl.h
SOURCES       =  bluezapplet.cpp bluezappletimpl.cpp
TARGET        =  bluetoothapplet
DESTDIR       =  $(OPIEDIR)/plugins/applets
INCLUDEPATH   += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopietooth
VERSION       = 0.0.3

