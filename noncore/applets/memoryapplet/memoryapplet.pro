TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = ../../settings/sysinfo/graph.h \
            ../../settings/sysinfo/load.h  \
            ../../settings/sysinfo/memory.h  \
            memorymeter.h  \
            memorystatus.h \
            swapfile.h
SOURCES     = ../../settings/sysinfo/graph.cpp \
            ../../settings/sysinfo/load.cpp \
            ../../settings/sysinfo/memory.cpp \
            memorymeter.cpp \
            memorystatus.cpp \
            swapfile.cpp
TARGET      = memoryapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += ../$(OPIEDIR)/include
VERSION     = 1.0.0
LIBS        += -lqpe -lopiecore2 -lopieui2

include ( $(OPIEDIR)/include.pro )
