CONFIG += qt warn_on
TEMPLATE = app
TARGET = owidgetstack-example

SOURCES = owidgetstack_example.cpp
HEADERS = owidgetstack_example.h

INCLUDEPATH += $(OPIEDIR)/include
DEPENDSPATH += $(OPIEDIR)/include

LIBS += -lqpe -lopieui2

include ( $(OPIEDIR)/include.pro )
