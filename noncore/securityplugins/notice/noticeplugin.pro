TEMPLATE = lib
CONFIG += qt warn_on

HEADERS		= noticeplugin.h noticepluginimpl.h noticeConfigWidget.h

SOURCES		= noticeplugin.cpp noticepluginimpl.cpp noticeConfigWidget.cpp


INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+=

LIBS		+= -lqpe -lopieui2

DESTDIR = $(OPIEDIR)/plugins/security
TARGET = multiauthnoticeplugin

include ( $(OPIEDIR)/include.pro )
