TEMPLATE = lib
CONFIG += qt warn_on

HEADERS		= bluepingplugin.h bluepingpluginimpl.h bluepingConfigWidget.h

SOURCES		= bluepingplugin.cpp bluepingpluginimpl.cpp bluepingConfigWidget.cpp


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+=

LIBS		+= -lqpe -lopieui2 -lopiecore2 -lopiesecurity2

DESTDIR = $(OPIEDIR)/plugins/security
TARGET = multiauthbluepingplugin

include( $(OPIEDIR)/include.pro )

