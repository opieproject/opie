TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release
# Input
HEADERS = datebookplugin.h datebookpluginimpl.h datebookpluginconfig.h \
	datebookevent.h datebookpluginwidget.h
SOURCES = datebookplugin.cpp datebookpluginimpl.cpp datebookpluginconfig.cpp \
	datebookevent.cpp datebookpluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
		../ ../library

LIBS+= -lqpe -lopie 

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaydatebookplugin