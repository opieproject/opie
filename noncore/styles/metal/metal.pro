TEMPLATE    =  lib
CONFIG       = qt embedded release warn_on
SOURCES     =  metal.cpp plugin.cpp
               
HEADERS     =  metal.h
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
#TARGET       = metal
TARGET = liquid
VERSION      = 1.0.0