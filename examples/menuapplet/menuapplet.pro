TEMPLATE      = lib
CONFIG       += qt plugn warn_on 
HEADERS       = example.h
SOURCES       = example.cpp
TARGET        = example_applet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

include ( $(OPIEDIR)/include.pro )
