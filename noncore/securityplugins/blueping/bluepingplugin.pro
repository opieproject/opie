TEMPLATE = lib
CONFIG += qt warn_on

HEADERS		= bluepingplugin.h bluepingpluginimpl.h

SOURCES		= bluepingplugin.cpp bluepingpluginimpl.cpp


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+=

LIBS		+= -lqpe -lopieui2 -lopiecore2

DESTDIR = $(OPIEDIR)/plugins/security
TARGET = multiauthbluepingplugin

include ( $(OPIEDIR)/include.pro )

