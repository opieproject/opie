TEMPLATE    =  lib
CONFIG       = qt plugin embedded release warn_on
SOURCES     =  liquid.cpp               
HEADERS     =  liquid.h               
LIBS        += -lqpe 
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/decorations
TARGET       = liquid
VERSION      = 1.0.0




include ( $(OPIEDIR)/include.pro )
