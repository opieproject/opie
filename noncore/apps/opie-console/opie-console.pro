TEMPLATE     = app
CONFIG       = qt warn_on release
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = io_layer.h io_serial.h file_layer.h
SOURCES      = io_layer.cpp io_serial.cpp file_layer.cpp
INTERFACES   = 
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = sysinfo

