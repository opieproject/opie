TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= soundsettings.h
SOURCES		= soundsettings.cpp main.cpp
INTERFACES	= soundsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= sound

TRANSLATIONS += ../../i18n/pt_BR/sound.ts
