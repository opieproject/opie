TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   screenshot.h screenshotappletimpl.h
SOURCES =   screenshot.cpp screenshotappletimpl.cpp
TARGET    = screenshotapplet
DESTDIR   = ../../plugins/applets
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += ../$(QPEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj

