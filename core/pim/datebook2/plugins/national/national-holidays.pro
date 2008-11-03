TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin

# Input
HEADERS = nationalholiday.h nationalcfg.h nationalcfgwidget.h
SOURCES = nationalholiday.cpp nationalcfg.cpp nationalcfgwidget.cpp
INTERFACES=

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/holidays
TARGET = nationaldayplugin

include( $(OPIEDIR)/include.pro )
