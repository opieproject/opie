TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = addressplugin.h addresspluginimpl.h addresspluginconfig.h  \
	addresspluginwidget.h
SOURCES = addressplugin.cpp addresspluginimpl.cpp addresspluginconfig.cpp \
	addresspluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayaddressbookplugin
