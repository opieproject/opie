TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin release

# Input
HEADERS = mailplugin.h mailpluginimpl.h mailpluginwidget.h
SOURCES = mailplugin.cpp mailpluginimpl.cpp mailpluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaymailplugin

include ( $(OPIEDIR)/include.pro )
