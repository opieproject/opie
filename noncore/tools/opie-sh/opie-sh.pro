TEMPLATE			=app
CONFIG				+=qt warn_on release
DESTDIR				=$(OPIEDIR)/bin
HEADERS				=mbox.h fviewer.h inputdialog.h
SOURCES				=opie-sh.cpp mbox.cpp fviewer.cpp inputdialog.cpp
INCLUDEPATH		+=$(OPIEDIR)/include
DEPENDPATH		+=$(OPIEDIR)/include
LIBS						+=-lqpe
TARGET                   = opie-sh

TRANSLATIONS = ../../../i18n/de/opie-sh.ts \
	 ../../../i18n/en/opie-sh.ts \
	 ../../../i18n/es/opie-sh.ts \
	 ../../../i18n/fr/opie-sh.ts \
	 ../../../i18n/hu/opie-sh.ts \
	 ../../../i18n/ja/opie-sh.ts \
	 ../../../i18n/ko/opie-sh.ts \
	 ../../../i18n/no/opie-sh.ts \
	 ../../../i18n/pl/opie-sh.ts \
	 ../../../i18n/pt/opie-sh.ts \
	 ../../../i18n/pt_BR/opie-sh.ts \
	 ../../../i18n/sl/opie-sh.ts \
	 ../../../i18n/zh_CN/opie-sh.ts \
	 ../../../i18n/zh_TW/opie-sh.ts
