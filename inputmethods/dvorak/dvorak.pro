TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   dvorak.h \
	    ../pickboard/pickboardcfg.h \
	    ../pickboard/pickboardpicks.h \
	    dvorakimpl.h 
SOURCES	=   dvorak.cpp \
	    ../pickboard/pickboardcfg.cpp \
	    ../pickboard/pickboardpicks.cpp \
	    dvorakimpl.cpp
TARGET		= qdvorak
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../taskbar
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS += ../../i18n/de/libqdvorak.ts
TRANSLATIONS  = ../../i18n/pt_BR/libqdvorak.ts

