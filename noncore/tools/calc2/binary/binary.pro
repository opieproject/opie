TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

# Input
INTERFACES += binary.ui
HEADERS = binaryimpl.h \
	    binaryfactory.h 
SOURCES = binaryimpl.cpp \
	    binaryfactory.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
		    $(OPIEDIR)/calc2
DEPENDPATH      += $(OPIEDIR)/include 

DESTDIR = $(OPIEDIR)/plugins/calculator


TRANSLATIONS = ../../../../i18n/de/lib.ts
TRANSLATIONS += ../../../../i18n/en/lib.ts
TRANSLATIONS += ../../../../i18n/es/lib.ts
TRANSLATIONS += ../../../../i18n/fr/lib.ts
TRANSLATIONS += ../../../../i18n/hu/lib.ts
TRANSLATIONS += ../../../../i18n/ja/lib.ts
TRANSLATIONS += ../../../../i18n/ko/lib.ts
TRANSLATIONS += ../../../../i18n/no/lib.ts
TRANSLATIONS += ../../../../i18n/pl/lib.ts
TRANSLATIONS += ../../../../i18n/pt/lib.ts
TRANSLATIONS += ../../../../i18n/pt_BR/lib.ts
TRANSLATIONS += ../../../../i18n/sl/lib.ts
TRANSLATIONS += ../../../../i18n/zh_CN/lib.ts
TRANSLATIONS += ../../../../i18n/zh_TW/lib.ts

