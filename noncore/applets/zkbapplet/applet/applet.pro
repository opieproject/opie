TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = zkbwidget.h \
            ../keyz-cfg/zkbcfg.h  \
            ../keyz-cfg/zkbnames.h  \
            ../keyz-cfg/zkbxml.h  \
            ../keyz-cfg/zkb.h  
SOURCES     = zkbwidget.cpp \
            ../keyz-cfg/zkbcfg.cpp  \
            ../keyz-cfg/zkbnames.cpp  \
            ../keyz-cfg/zkbxml.cpp  \
            ../keyz-cfg/zkb.cpp  
TARGET      = zkbapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include ../keyz-cfg
DEPENDPATH  += 
VERSION     = 0.7.0
LIBS        += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )
