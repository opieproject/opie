TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
INTERFACES += simple.ui
HEADERS = simpleimpl.h  simplefactory.h stdinstructions.h
SOURCES = simpleimpl.cpp simplefactory.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		    $(OPIEDIR)/calc2
DEPENDPATH      += $(OPIEDIR)/include 

DESTDIR = $(OPIEDIR)/plugins/calculator
