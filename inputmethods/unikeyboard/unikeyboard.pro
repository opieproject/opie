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

TRANSLATIONS = ../../i18n/de/libqunikeyboard.ts \
	 ../../i18n/en/libqunikeyboard.ts \
	 ../../i18n/es/libqunikeyboard.ts \
	 ../../i18n/fr/libqunikeyboard.ts \
	 ../../i18n/hu/libqunikeyboard.ts \
	 ../../i18n/ja/libqunikeyboard.ts \
	 ../../i18n/ko/libqunikeyboard.ts \
	 ../../i18n/no/libqunikeyboard.ts \
	 ../../i18n/pl/libqunikeyboard.ts \
	 ../../i18n/pt/libqunikeyboard.ts \
	 ../../i18n/pt_BR/libqunikeyboard.ts \
	 ../../i18n/sl/libqunikeyboard.ts \
	 ../../i18n/zh_CN/libqunikeyboard.ts \
	 ../../i18n/zh_TW/libqunikeyboard.ts
