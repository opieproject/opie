TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
INTERFACES += binary.ui
HEADERS = binaryimpl.h \
	    binaryfactory.h 
SOURCES = binaryimpl.cpp \
	    binaryfactory.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		    $(OPIEDIR)/calc2
DEPENDPATH      += $(OPIEDIR)/include 

DESTDIR = $(OPIEDIR)/plugins/calculator
