CONFIG += qt warn_on quick-app


TARGET = simple-pim

HEADERS = simple.h
SOURCES = simple.cpp


INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include


# we now also include opie
LIBS += -lqpe -lopiecore2 -lopiepim2 -lopieui2

include ( $(OPIEDIR)/include.pro )
