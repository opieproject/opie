TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   pickboard.h pickboardcfg.h pickboardimpl.h pickboardpicks.h
SOURCES	=   pickboard.cpp pickboardcfg.cpp pickboardimpl.cpp pickboardpicks.cpp
TARGET		= qpickboard
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../taskbar
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libqpickboard.ts
TRANSLATIONS += ../../i18n/pt_BR/libqpickboard.ts
