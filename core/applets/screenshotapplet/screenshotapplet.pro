TEMPLATE  = lib
CONFIG    += qt plugin warn_on release
HEADERS   = screenshot.h inputDialog.h
SOURCES   = screenshot.cpp inputDialog.cpp
TARGET    = screenshotapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
