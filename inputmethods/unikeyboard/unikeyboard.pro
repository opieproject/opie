TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   unikeyboard.h unikeyboardimpl.h
SOURCES	=   unikeyboard.cpp unikeyboardimpl.cpp
TARGET		= qunikeyboard
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libqunikeyboard.ts
TRANSLATIONS += ../../i18n/pt_BR/libqunikeyboard.ts
