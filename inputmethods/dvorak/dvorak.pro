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
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS    = ../../i18n/pt_BR/libqdvorak.ts
TRANSLATIONS   += ../../i18n/de/libqdvorak.ts
TRANSLATIONS   += ../../i18n/en/libqdvorak.ts
TRANSLATIONS   += ../../i18n/hu/libqdvorak.ts
TRANSLATIONS   += ../../i18n/sl/libqdvorak.ts
TRANSLATIONS   += ../../i18n/ja/libqdvorak.ts
TRANSLATIONS   += ../../i18n/ko/libqdvorak.ts
TRANSLATIONS   += ../../i18n/no/libqdvorak.ts
TRANSLATIONS   += ../../i18n/zh_CN/libqdvorak.ts
TRANSLATIONS   += ../../i18n/zh_TW/libqdvorak.ts
TRANSLATIONS   += ../../i18n/fr/libqdvorak.ts
