TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin

# Input
HEADERS = dummyholiday.h
SOURCES = dummyholiday.cpp
INTERFACES=

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2

DESTDIR = $(OPIEDIR)/plugins/datebook/holiday
TARGET = dummyholidayplugin

include( $(OPIEDIR)/include.pro )
