TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= settings.h
SOURCES		= light.cpp main.cpp
INTERFACES	= lightsettingsbase.ui
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= ../$(QPEDIR)/include
LIBS            += -lqpe
TARGET		= light-and-power

TRANSLATIONS   = ../../i18n/de/light-and-power.ts
