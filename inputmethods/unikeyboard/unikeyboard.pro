TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   unikeyboard.h unikeyboardimpl.h
SOURCES	=   unikeyboard.cpp unikeyboardimpl.cpp
TARGET		= qunikeyboard
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH      += ../$(QPEDIR)/include ../../taskbar
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS += ../../i18n/de/libqunikeyboard.ts
