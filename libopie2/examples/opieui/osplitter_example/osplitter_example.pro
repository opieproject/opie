CONFIG = qt warn_on 
TEMPLATE = app
TARGET = osplitter_example

HEADERS = osplitter_example.h
SOURCES = osplitter_example.cpp

INCLUDEPATH +=  $(OPIEDIR)/include
DEPENDSPATH +=  $(OPIEDIR)/include

LIBS += -lqpe -lopieui2

include ( $(OPIEDIR)/include.pro )
