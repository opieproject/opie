TEMPLATE     = lib
CONFIG      += qt plugin warn_on release
HEADERS      = wireless.h networkinfo.h mgraph.h advancedconfig.h
SOURCES      = wireless.cpp networkinfo.cpp mgraph.cpp advancedconfig.cpp
INTERFACES   = advancedconfigbase.ui
TARGET       = wirelessapplet
DESTDIR      = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe
VERSION      = 0.1.1

include ( $(OPIEDIR)/include.pro )
