TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = todoplugin.h todopluginimpl.h todopluginconfig.h  \
	todopluginwidget.h
SOURCES = todoplugin.cpp todopluginimpl.cpp todopluginconfig.cpp \
	todopluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaytodolistplugin