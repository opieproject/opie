TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= settings.h
SOURCES		= light.cpp main.cpp
INTERFACES	= lightsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= light-and-power

TRANSLATIONS   = ../../i18n/de/light-and-power.ts
TRANSLATIONS   += ../../i18n/pt_BR/light-and-power.ts
TRANSLATIONS   += ../../i18n/pl/light-and-power.ts
TRANSLATIONS   += ../../i18n/fr/light-and-power.ts
