TEMPLATE	=	lib
CONFIG		+=	qt warn_on release
HEADERS		+=	bend.h \
			bendimpl.h
SOURCES		+=	bend.cpp \
			bendimpl.cpp
INCLUDEPATH	+=	$(OPIEDIR)/include ../libmail
LIBS		+=	-lmail -lqpe
TARGET		=	bend
DESTDIR		+=	$(OPIEDIR)/plugins/applets/


TRANSLATIONS = ../../../i18n/de/libbend.ts
TRANSLATIONS += ../../../i18n/en/libbend.ts
TRANSLATIONS += ../../../i18n/es/libbend.ts
TRANSLATIONS += ../../../i18n/fr/libbend.ts
TRANSLATIONS += ../../../i18n/hu/libbend.ts
TRANSLATIONS += ../../../i18n/ja/libbend.ts
TRANSLATIONS += ../../../i18n/ko/libbend.ts
TRANSLATIONS += ../../../i18n/no/libbend.ts
TRANSLATIONS += ../../../i18n/pl/libbend.ts
TRANSLATIONS += ../../../i18n/pt/libbend.ts
TRANSLATIONS += ../../../i18n/pt_BR/libbend.ts
TRANSLATIONS += ../../../i18n/sl/libbend.ts
TRANSLATIONS += ../../../i18n/zh_CN/libbend.ts
TRANSLATIONS += ../../../i18n/zh_TW/libbend.ts

