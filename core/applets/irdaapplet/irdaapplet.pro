TEMPLATE  	= lib
CONFIG    	+= qt plugin warn_on 
HEADERS 	= irda.h
SOURCES 	= irda.cpp
TARGET    	= irdaapplet
DESTDIR   	= $(OPIEDIR)/plugins/applets
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
