TEMPLATE    =  lib
CONFIG      = qt plugin embedded warn_on
SOURCES     =  liquid.cpp \
               effects.cpp \
               liquidset.cpp \
               plugin.cpp
               
HEADERS     =  liquid.h \
               effects.h \
               liquidset.h \
               plugin.h
               
LIBS        += -lqpe -lqtaux2 -lopiecore2
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = liquid
VERSION      = 1.0.1

include ( $(OPIEDIR)/include.pro )
