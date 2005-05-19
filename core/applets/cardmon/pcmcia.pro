TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = pcmcia.h
SOURCES     = pcmcia.cpp
TARGET      = pcmciaapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
VERSION     = 0.1.0

include( $(OPIEDIR)/include.pro )
