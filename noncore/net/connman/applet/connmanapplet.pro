TEMPLATE      =  lib
CONFIG        += qt plugin warn_on 
HEADERS       =  connmanapplet.h
SOURCES       =  connmanapplet.cpp
TARGET        =  connmanapplet
DESTDIR       =  $(OPIEDIR)/plugins/applets
INCLUDEPATH   += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopiecore2 -ldbus-qt2 $$system(pkg-config --libs dbus-1)
VERSION       = 0.0.1

include( $(OPIEDIR)/include.pro )
