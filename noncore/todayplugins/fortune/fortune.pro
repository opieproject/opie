TEMPLATE = lib
#CONFIG -= moc
CONFIG += qt debug

# Input
HEADERS = fortuneplugin.h fortunepluginimpl.h \
	fortunepluginwidget.h
SOURCES = fortuneplugin.cpp fortunepluginimpl.cpp \
	fortunepluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayfortuneplugin
