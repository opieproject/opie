TEMPLATE    =  lib
CONFIG      = qt plugin embedded warn_on
SOURCES     =   webstyle.cpp \
		plugin.cpp 
HEADERS     =   webstyle.h \
		plugin.h
               
LIBS        += -lqpe 
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = webstyle
VERSION      = 0.4.0

include ( $(OPIEDIR)/include.pro )
