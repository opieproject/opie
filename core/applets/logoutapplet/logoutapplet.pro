TEMPLATE      = lib
CONFIG       += qt warn_on release
HEADERS       = logout.h
SOURCES       = logout.cpp
TARGET        = logoutapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

include ( $(OPIEDIR)/include.pro )
