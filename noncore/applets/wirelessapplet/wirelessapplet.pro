TEMPLATE     = lib
CONFIG      += qt plugin warn_on 
HEADERS      = wireless.h mgraph.h advancedconfig.h
SOURCES      = wireless.cpp mgraph.cpp advancedconfig.cpp
INTERFACES   = advancedconfigbase.ui
TARGET       = wirelessapplet
DESTDIR      = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopienet2
VERSION      = 0.1.1

include ( $(OPIEDIR)/include.pro )
