TEMPLATE			=app
CONFIG				+=qt warn_on release
DESTDIR				=$(OPIEDIR)/bin
HEADERS				=mbox.h fviewer.h inputdialog.h
SOURCES				=opie-sh.cpp mbox.cpp fviewer.cpp inputdialog.cpp
INCLUDEPATH		+=$(OPIEDIR)/include
DEPENDPATH		+=$(OPIEDIR)/include
LIBS						+=-lqpe
TARGET                   = opie-sh


TRANSLATIONS = ../../../i18n/de/opie-sh.ts
TRANSLATIONS += ../../../i18n/en/opie-sh.ts
TRANSLATIONS += ../../../i18n/es/opie-sh.ts
TRANSLATIONS += ../../../i18n/fr/opie-sh.ts
TRANSLATIONS += ../../../i18n/hu/opie-sh.ts
TRANSLATIONS += ../../../i18n/ja/opie-sh.ts
TRANSLATIONS += ../../../i18n/ko/opie-sh.ts
TRANSLATIONS += ../../../i18n/no/opie-sh.ts
TRANSLATIONS += ../../../i18n/pl/opie-sh.ts
TRANSLATIONS += ../../../i18n/pt/opie-sh.ts
TRANSLATIONS += ../../../i18n/pt_BR/opie-sh.ts
TRANSLATIONS += ../../../i18n/sl/opie-sh.ts
TRANSLATIONS += ../../../i18n/zh_CN/opie-sh.ts
TRANSLATIONS += ../../../i18n/zh_TW/opie-sh.ts

