TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   screenshot.h screenshotappletimpl.h
SOURCES =   screenshot.cpp screenshotappletimpl.cpp
TARGET    = screenshotapplet
DESTDIR   = ../../plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj

TRANSLATIONS = ../../i18n/pt_BR/libscreenshotapplet.ts
