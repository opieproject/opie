TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = brightnessapplet.h
SOURCES     = brightnessapplet.cpp
TARGET      = brightnessapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2
VERSION     = 0.1.1


include ( $(OPIEDIR)/include.pro )
