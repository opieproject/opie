TEMPLATE    =  lib

CONFIG       = qt embedded release warn_on

SOURCES     =  liquid.cpp \
               effects.cpp \
               plugin.cpp
               
HEADERS     =  liquid.h \
               effects.h

LIBS       += -lqpe

INCLUDEPATH += $(OPIEDIR)/include

DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = liquid

VERSION      = 1.0.0