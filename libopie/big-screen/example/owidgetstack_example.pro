CONFIG += qt warn_on
TEMPLATE = app
TARGET = owidgetstack-example

SOURCES = ../owidgetstack.cpp owidgetstack_example.cpp
HEADERS = ../owidgetstack.h owidgetstack_example.h

INCLUDEPATH += $(OPIEDIR)/include
DEPENDSPATH += $(OPIEDIR)/include

LIBS += -lqpe

include ( $(OPIEDIR)/include.pro )
