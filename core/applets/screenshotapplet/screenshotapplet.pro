TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   screenshot.h inputDialog.h screenshotappletimpl.h
SOURCES =   screenshot.cpp inputDialog.cpp screenshotappletimpl.cpp
TARGET    = screenshotapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj

include ( $(OPIEDIR)/include.pro )
