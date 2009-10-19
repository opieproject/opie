TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin

# Input
HEADERS = chrisholiday.h hlist.h
SOURCES = chrisholiday.cpp hlist.cpp
INTERFACES=

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/holidays2
TARGET = chrisholidayplugin

include( $(OPIEDIR)/include.pro )
