TEMPLATE    =  lib
CONFIG       = qt embedded release warn_on
SOURCES     =  metal.cpp plugin.cpp
               
HEADERS     =  metal.h plugin.h
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = metal
VERSION      = 1.0.0

include ( $(OPIEDIR)/include.pro )
