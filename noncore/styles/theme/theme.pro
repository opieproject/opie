TEMPLATE    =  lib
CONFIG       = qt embedded debug warn_on
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

               
LIBS       += -lqpe
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/plugins/styles
TARGET       = theme
VERSION      = 1.0.0

include ( $(OPIEDIR)/include.pro )
