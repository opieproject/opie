TEMPLATE = lib
CONFIG -= moc
CONFIG += qt debug

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