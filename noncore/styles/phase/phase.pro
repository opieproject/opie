TEMPLATE    =  lib
CONFIG      = qt plugin embedded warn_on
SOURCES     =   phasestyle.cpp \
		plugin.cpp 
HEADERS     =   bitmaps.h \
		phasestyle.h \
		plugin.h
               
LIBS        += -lqpe 
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = phase
VERSION      = 0.4.0

include ( $(OPIEDIR)/include.pro )
