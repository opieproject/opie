TEMPLATE  = lib
CONFIG    += qt plugin warn_on release
HEADERS =   notes.h
SOURCES =   notes.cpp
TARGET    = notesapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

include ( $(OPIEDIR)/include.pro )
