TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin 
# Input
HEADERS = datebookplugin.h datebookpluginimpl.h datebookpluginconfig.h \
	datebookevent.h datebookpluginwidget.h
SOURCES = datebookplugin.cpp datebookpluginimpl.cpp datebookpluginconfig.cpp \
	datebookevent.cpp datebookpluginwidget.cpp
INTERFACES = datebookpluginconfigbase.ui

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaydatebookplugin

include ( $(OPIEDIR)/include.pro )
