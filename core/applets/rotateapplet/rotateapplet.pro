TEMPLATE      = lib
CONFIG       += qt plugn warn_on 
HEADERS       = rotate.h
SOURCES       = rotate.cpp
TARGET        = rotateapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe -lopiecore2
VERSION       = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
