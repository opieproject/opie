CONFIG += qt warn_on quick-app


TARGET = simple-pim

HEADERS = simple.h
SOURCES = simple.cpp


INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include


# we now also include opie
LIBS += -lqpe -lopie

include ( $(OPIEDIR)/include.pro )
