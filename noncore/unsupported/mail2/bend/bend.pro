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

TRANSLATIONS = ../../../i18n/de/libbend.ts \
	 ../../../i18n/en/libbend.ts \
	 ../../../i18n/es/libbend.ts \
	 ../../../i18n/fr/libbend.ts \
	 ../../../i18n/hu/libbend.ts \
	 ../../../i18n/ja/libbend.ts \
	 ../../../i18n/ko/libbend.ts \
	 ../../../i18n/no/libbend.ts \
	 ../../../i18n/pl/libbend.ts \
	 ../../../i18n/pt/libbend.ts \
	 ../../../i18n/pt_BR/libbend.ts \
	 ../../../i18n/sl/libbend.ts \
	 ../../../i18n/zh_CN/libbend.ts \
	 ../../../i18n/zh_TW/libbend.ts
