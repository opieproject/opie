TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = graph.h \
            load.h \
            memory.h \
            memorymeter.h  \
            memorystatus.h \
            swapfile.h
SOURCES     = graph.cpp \
            load.cpp \
            memory.cpp \
            memorymeter.cpp \
            memorystatus.cpp \
            swapfile.cpp
TARGET      = memoryapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += 
VERSION     = 1.0.1
LIBS        += -lqpe -lopiecore2 -lopieui2

include( $(OPIEDIR)/include.pro )
