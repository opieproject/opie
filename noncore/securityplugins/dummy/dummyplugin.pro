TEMPLATE = lib
CONFIG += qt warn_on
CONFIG -= moc

HEADERS		= dummyplugin.h dummypluginimpl.h

SOURCES		= dummyplugin.cpp dummypluginimpl.cpp


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+=

LIBS		+= -lqpe -lopieui2

DESTDIR = $(OPIEDIR)/plugins/security
TARGET = multiauthdummyplugin

include ( $(OPIEDIR)/include.pro )

