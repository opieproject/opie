TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin 

# Input
HEADERS = todoplugin.h todopluginimpl.h todopluginconfig.h  \
	todopluginwidget.h
SOURCES = todoplugin.cpp todopluginimpl.cpp todopluginconfig.cpp \
	todopluginwidget.cpp
INTERFACES= todopluginconfigbase.ui

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaytodolistplugin

include ( $(OPIEDIR)/include.pro )
