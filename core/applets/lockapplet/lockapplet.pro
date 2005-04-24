TEMPLATE      = lib
CONFIG       += qt plugn warn_on 
HEADERS       = lock.h
SOURCES       = lock.cpp
TARGET        = lockapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe -lopiecore2 -lopiesecurity2
VERSION       = 1.0.0

include( $(OPIEDIR)/include.pro )
