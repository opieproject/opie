TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin

# Input
HEADERS = nationalholiday.h nationalcfg.h
SOURCES = nationalholiday.cpp nationalcfg.cpp
INTERFACES=

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2

DESTDIR = $(OPIEDIR)/plugins/datebook/holiday
TARGET = nationaldayplugin

include( $(OPIEDIR)/include.pro )
