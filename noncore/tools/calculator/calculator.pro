DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= calculatorimpl.h
SOURCES		= calculatorimpl.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= calculator.ui
TARGET		= calculator

TRANSLATIONS = ../../../i18n/de/calculator.ts \
	 ../../../i18n/en/calculator.ts \
	 ../../../i18n/es/calculator.ts \
	 ../../../i18n/fr/calculator.ts \
	 ../../../i18n/hu/calculator.ts \
	 ../../../i18n/ja/calculator.ts \
	 ../../../i18n/ko/calculator.ts \
	 ../../../i18n/no/calculator.ts \
	 ../../../i18n/pl/calculator.ts \
	 ../../../i18n/pt/calculator.ts \
	 ../../../i18n/pt_BR/calculator.ts \
	 ../../../i18n/sl/calculator.ts \
	 ../../../i18n/zh_CN/calculator.ts \
	 ../../../i18n/zh_TW/calculator.ts
