TEMPLATE      = lib
CONFIG       += qt warn_on release
HEADERS       = vt.h
SOURCES       = vt.cpp
TARGET        = vtapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
