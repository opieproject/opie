TEMPLATE = app
CONFIG += qt warn_on

SOURCES = oplugins.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDSPATH += $(OPIEDIR)/include

LIBS += -lopiecore2

include ( $(OPIEDIR)/include.pro )
