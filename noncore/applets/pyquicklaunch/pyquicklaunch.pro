TEMPLATE     = lib
CONFIG      += qt plugin warn_on
HEADERS      = pyquicklaunch.h
SOURCES      = pyquicklaunch.cpp
INTERFACES   =
TARGET       = pyquicklaunch
DESTDIR      = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
VERSION      = 0.0.1

include ( $(OPIEDIR)/include.pro )
