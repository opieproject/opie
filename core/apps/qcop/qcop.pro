TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= 
SOURCES		= main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= 
TARGET		= qcop

TRANSLATIONS = ../../../i18n/de/qcop.ts \
	 ../../../i18n/da/qcop.ts \
	 ../../../i18n/xx/qcop.ts \
	 ../../../i18n/en/qcop.ts \
	 ../../../i18n/es/qcop.ts \
	 ../../../i18n/fr/qcop.ts \
	 ../../../i18n/hu/qcop.ts \
	 ../../../i18n/ja/qcop.ts \
	 ../../../i18n/ko/qcop.ts \
	 ../../../i18n/no/qcop.ts \
	 ../../../i18n/pl/qcop.ts \
	 ../../../i18n/pt/qcop.ts \
	 ../../../i18n/pt_BR/qcop.ts \
	 ../../../i18n/sl/qcop.ts \
	 ../../../i18n/zh_CN/qcop.ts \
	 ../../../i18n/zh_TW/qcop.ts
