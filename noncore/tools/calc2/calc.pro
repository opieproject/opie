TEMPLATE = app
CONFIG += qt release

# Input
HEADERS += calc.h plugininterface.h instruction.h engine.h stdinstructions.h
SOURCES += calc.cpp main.cpp engine.cpp

INCLUDEPATH     += $(QPEDIR)/include 
DEPENDPATH      += $(QPEDIR)/include 
LIBS += -lqpe -Wl,-export-dynamic 

