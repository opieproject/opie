TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin

# Input
HEADERS = birthday.h
SOURCES = birthday.cpp
INTERFACES=

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2

DESTDIR = $(OPIEDIR)/plugins/holidays
TARGET = birthdayplugin

include( $(OPIEDIR)/include.pro )
