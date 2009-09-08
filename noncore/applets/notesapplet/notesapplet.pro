TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = notes.h notesmanager.h
SOURCES     = notes.cpp notesmanager.cpp
TARGET      = notesapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopiepim2
VERSION   = 1.0.1

include( $(OPIEDIR)/include.pro )
