CONFIG = qt warn_on 
TEMPLATE = app
TARGET = osplitter_example

HEADERS = osplitter_example.h ../osplitter.h
SOURCES = osplitter_example.cpp ../osplitter.cpp

INCLUDEPATH +=  $(OPIEDIR)/include ../
DEPENDSPATH +=  $(OPIEDIR)/include

LIBS += -lqpe -lopie

include ( $(OPIEDIR)/include.pro )