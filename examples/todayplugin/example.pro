TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
HEADERS = exampleplugin.h examplepluginimpl.h examplepluginwidget.h
SOURCES = exampleplugin.cpp examplepluginimpl.cpp examplepluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include 

LIBS+= -lqpe -lopieui2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayexampleplugin

include ( $(OPIEDIR)/include.pro )
