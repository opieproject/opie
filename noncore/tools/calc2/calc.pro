TEMPLATE = app
CONFIG += qt release

# Input
HEADERS += calc.h plugininterface.h instruction.h engine.h stdinstructions.h
SOURCES += calc.cpp main.cpp engine.cpp

INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include 
LIBS += -lqpe -Wl,-export-dynamic 

