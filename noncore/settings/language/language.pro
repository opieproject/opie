TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= settings.h
SOURCES		= language.cpp main.cpp
INTERFACES	= languagesettingsbase.ui
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= ../$(QPEDIR)/include
LIBS            += -lqpe
TARGET		= language

TRANSLATIONS    = ../../i18n/de/language.ts 
