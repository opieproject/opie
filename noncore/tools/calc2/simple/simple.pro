TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release
# Input
INTERFACES += simple.ui
HEADERS = simpleimpl.h  simplefactory.h stdinstructions.h
SOURCES = simpleimpl.cpp simplefactory.cpp
INCLUDEPATH     += $(OPIEDIR)/include \
		    $(OPIEDIR)/calc2
DEPENDPATH      += $(OPIEDIR)/include 
DESTDIR = $(OPIEDIR)/plugins/calculator

TRANSLATIONS = ../../../../i18n/de/lib.ts \
	 ../../../../i18n/en/lib.ts \
	 ../../../../i18n/es/lib.ts \
	 ../../../../i18n/fr/lib.ts \
	 ../../../../i18n/hu/lib.ts \
	 ../../../../i18n/ja/lib.ts \
	 ../../../../i18n/ko/lib.ts \
	 ../../../../i18n/no/lib.ts \
	 ../../../../i18n/pl/lib.ts \
	 ../../../../i18n/pt/lib.ts \
	 ../../../../i18n/pt_BR/lib.ts \
	 ../../../../i18n/sl/lib.ts \
	 ../../../../i18n/zh_CN/lib.ts \
	 ../../../../i18n/zh_TW/lib.ts
