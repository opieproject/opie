TEMPLATE      = lib
CONFIG       += qt warn_on release
HEADERS       = suspend.h
SOURCES       = suspend.cpp
TARGET        = suspendapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

include ( $(OPIEDIR)/include.pro )
