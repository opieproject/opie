TEMPLATE    =  lib
CONFIG       = qt plugin embedded  warn_on
SOURCES     =  ogfxeffect.cpp \
               othemestyle.cpp \
               othemebase.cpp \
               themeset.cpp \
               plugin.cpp

HEADERS     = ogfxeffect.h \
              othemebase.h \
              othemestyle.h \
              themeset.h \
              plugin.h

               
LIBS       += -lqpe -lopiecore2
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = theme
VERSION      = 1.0.1

include ( $(OPIEDIR)/include.pro )
