TEMPLATE = lib
#TEMPLATE = app
CONFIG -= moc
CONFIG += qt debug

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