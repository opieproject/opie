CONFIG += qt warn_on quick-app


TARGET = simple-main

HEADERS = simple.h
SOURCES = simple.cpp


INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include



LIBS += -lqpe 

include ( $(OPIEDIR)/include.pro )
