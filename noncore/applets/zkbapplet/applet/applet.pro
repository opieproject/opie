TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = zkbwidget.h \
            ../keyzcfg/zkbcfg.h  \
            ../keyzcfg/zkbnames.h  \
            ../keyzcfg/zkbxml.h  \
            ../keyzcfg/zkb.h  
SOURCES     = zkbwidget.cpp \
            ../keyzcfg/zkbcfg.cpp  \
            ../keyzcfg/zkbnames.cpp  \
            ../keyzcfg/zkbxml.cpp  \
            ../keyzcfg/zkb.cpp  
TARGET      = zkbapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include ../keyzcfg
DEPENDPATH  += 
VERSION     = 0.7.0
LIBS        += -lqpe -lopiecore2

include( $(OPIEDIR)/include.pro )
