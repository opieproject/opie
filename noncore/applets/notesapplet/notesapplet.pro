TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = notes.h
SOURCES     = notes.cpp
TARGET      = notesapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
VERSION   = 1.0.1

include ( $(OPIEDIR)/include.pro )
