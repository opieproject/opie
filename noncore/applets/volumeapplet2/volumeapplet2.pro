TEMPLATE    = lib
CONFIG      += qt plugin warn_on
HEADERS     = volumeapplet.h
SOURCES     = volumeapplet.cpp
TARGET      = volumeapplet2
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2 -lopiemm2
VERSION     = 0.1.0

include( $(OPIEDIR)/include.pro )
