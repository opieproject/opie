CONFIG += qt warn_on quick-app


TARGET = main-tab

HEADERS = simple.h
SOURCES = simple.cpp


INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include


# we now also include opie
LIBS += -lqpe -lopieui2

include ( $(OPIEDIR)/include.pro )
