TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
INTERFACES += simple.ui
HEADERS = simpleimpl.h  simplefactory.h stdinstructions.h
SOURCES = simpleimpl.cpp simplefactory.cpp

INCLUDEPATH     += $(QPEDIR)/include \
		    $(QPEDIR)/calc2
DEPENDPATH      += $(QPEDIR)/include 

DESTDIR = $(QPEDIR)/plugins/calculator
