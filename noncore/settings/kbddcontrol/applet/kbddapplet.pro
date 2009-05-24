CONFIG += warn_on qt
 
TEMPLATE = lib
DESTDIR = $(OPIEDIR)/plugins/applets
TARGET  = kbddapplet

SOURCES = kbddapplet.cpp ../kbddhandler.cpp
HEADERS = kbddapplet.h ../kbddhandler.h

INCLUDEPATH += $(OPIEDIR)/include
DEPENDSPATH += $(OPIEDIR)/include

LIBS += -lqpe -lopieui2

include( $(OPIEDIR)/include.pro ) 
target.path = $$prefix/plugins/applets
