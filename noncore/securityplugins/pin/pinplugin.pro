VERSION		= 0.0.2
TEMPLATE = lib
CONFIG += qt warn_on

HEADERS		= pin.h pinpluginimpl.h pinConfigWidget.h

INTERFACES      = pinDialogBase.ui

SOURCES		= pin.cpp pinpluginimpl.cpp pinConfigWidget.cpp


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+=

LIBS		+= -lqpe -lopieui2

DESTDIR = $(OPIEDIR)/plugins/security
TARGET = multiauthpinplugin

include ( $(OPIEDIR)/include.pro )

