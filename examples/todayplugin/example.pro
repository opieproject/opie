TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = exampleplugin.h examplepluginimpl.h examplepluginwidget.h
SOURCES = exampleplugin.cpp examplepluginimpl.cpp examplepluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayexampleplugin

include ( $(OPIEDIR)/include.pro )
