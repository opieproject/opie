CONFIG += qt warn_on quick-app
HEADERS         = multiauthconfig.h ownerInfoConfigWidget.h
SOURCES         = multiauthconfig.cpp main.cpp ownerInfoConfigWidget.cpp
INTERFACES	= loginbase.ui syncbase.ui

INCLUDEPATH     += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lopiesecurity2
TARGET = security

include( $(OPIEDIR)/include.pro )
