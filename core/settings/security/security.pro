CONFIG += qt warn_on quick-app
HEADERS         = multiauthconfig.h
SOURCES         = multiauthconfig.cpp main.cpp
INTERFACES	= loginbase.ui syncbase.ui

INCLUDEPATH     += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lopiesecurity2
TARGET = security

include( $(OPIEDIR)/include.pro )
