TEMPLATE = lib
CONFIG += qt 
INTERFACES += binary.ui
HEADERS = binaryimpl.h \
	    binaryfactory.h 
SOURCES = binaryimpl.cpp \
	    binaryfactory.cpp
INCLUDEPATH     += $(OPIEDIR)/include \
		    $(OPIEDIR)/calc2
DEPENDPATH      += $(OPIEDIR)/include 
DESTDIR = $(OPIEDIR)/plugins/calculator
TARGET = binary


include( $(OPIEDIR)/include.pro )
