TEMPLATE    = lib
CONFIG      += qt plugin warn_on release
HEADERS     = ../../apps/sysinfo/graph.h \
            ../../apps/sysinfo/load.h  \
            ../../apps/sysinfo/memory.h  \
            memoryappletimpl.h  \
            memorymeter.h  \
            memorystatus.h \
            swapfile.h
SOURCES     = ../../apps/sysinfo/graph.cpp \
            ../../apps/sysinfo/load.cpp \
            ../../apps/sysinfo/memory.cpp \
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
