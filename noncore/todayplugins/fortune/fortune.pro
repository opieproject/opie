TEMPLATE = lib
CONFIG += qt plugin 

# Input
HEADERS = fortuneplugin.h fortunepluginimpl.h \
	fortunepluginwidget.h
SOURCES = fortuneplugin.cpp fortunepluginimpl.cpp \
	fortunepluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopiepim2 -lopieui2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayfortuneplugin

include ( $(OPIEDIR)/include.pro )
