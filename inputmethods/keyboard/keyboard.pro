TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   keyboard.h \
	    ../pickboard/pickboardcfg.h \
	    ../pickboard/pickboardpicks.h \
	    keyboardimpl.h 
SOURCES	=   keyboard.cpp \
	    ../pickboard/pickboardcfg.cpp \
	    ../pickboard/pickboardpicks.cpp \
	    keyboardimpl.cpp
TARGET		= qkeyboard
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0



TRANSLATIONS = ../../i18n/de/libqkeyboard.ts
TRANSLATIONS += ../../i18n/en/libqkeyboard.ts
TRANSLATIONS += ../../i18n/es/libqkeyboard.ts
TRANSLATIONS += ../../i18n/fr/libqkeyboard.ts
TRANSLATIONS += ../../i18n/hu/libqkeyboard.ts
TRANSLATIONS += ../../i18n/ja/libqkeyboard.ts
TRANSLATIONS += ../../i18n/ko/libqkeyboard.ts
TRANSLATIONS += ../../i18n/no/libqkeyboard.ts
TRANSLATIONS += ../../i18n/pl/libqkeyboard.ts
TRANSLATIONS += ../../i18n/pt/libqkeyboard.ts
TRANSLATIONS += ../../i18n/pt_BR/libqkeyboard.ts
TRANSLATIONS += ../../i18n/sl/libqkeyboard.ts
TRANSLATIONS += ../../i18n/zh_CN/libqkeyboard.ts
TRANSLATIONS += ../../i18n/zh_TW/libqkeyboard.ts

