TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
INTERFACES += binary.ui
HEADERS = binaryimpl.h \
	    binaryfactory.h 
SOURCES = binaryimpl.cpp \
	    binaryfactory.cpp

INCLUDEPATH     += $(QPEDIR)/include \
		    $(QPEDIR)/calc2
DEPENDPATH      += $(QPEDIR)/include 

DESTDIR = $(QPEDIR)/plugins/calculator
