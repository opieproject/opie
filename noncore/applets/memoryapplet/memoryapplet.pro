TEMPLATE    = lib
CONFIG      += qt plugin warn_on release
HEADERS     = ../../settings/sysinfo/graph.h \
            ../../settings/sysinfo/load.h  \
            ../../settings/sysinfo/memory.h  \
            memoryappletimpl.h  \
            memorymeter.h  \
            memorystatus.h \
            swapfile.h
SOURCES     = ../../settings/sysinfo/graph.cpp \
            ../../settings/sysinfo/load.cpp \
            ../../settings/sysinfo/memory.cpp \
            memoryappletimpl.cpp  \
            memorymeter.cpp \
            memorystatus.cpp \
            swapfile.cpp
TARGET      = memoryapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += ../$(OPIEDIR)/include
VERSION     = 1.0.0
LIBS        += -lqpe -lopie

include ( $(OPIEDIR)/include.pro )
