TEMPLATE	=	lib
CONFIG		=	qt warn_on debug
#CONFIG		=	qt warn_on release
HEADERS		=	configfile.h \
			defines.h \
			imapbase.h \
			imaphandler.h \
			imapresponse.h \
			mailfactory.h \
			md5.cpp \
			miscfunctions.h \
			smtphandler.h
SOURCES		=	configfile.cpp \
			imapbase.cpp \
			imaphandler.cpp \
			imapresponse.cpp \
			mailfactory.cpp \
			md5.h \
			miscfunctions.cpp \
			smtphandler.cpp
INCLUDEPATH	+=	$(OPIEDIR)/include
LIBS		+=	-L$(OPIEDIR)/lib -lqpe
TARGET		=	mail
DESTDIR		=	$(QTDIR)/lib


TRANSLATIONS = ../../../i18n/de/libmail.ts
TRANSLATIONS += ../../../i18n/en/libmail.ts
TRANSLATIONS += ../../../i18n/es/libmail.ts
TRANSLATIONS += ../../../i18n/fr/libmail.ts
TRANSLATIONS += ../../../i18n/hu/libmail.ts
TRANSLATIONS += ../../../i18n/ja/libmail.ts
TRANSLATIONS += ../../../i18n/ko/libmail.ts
TRANSLATIONS += ../../../i18n/no/libmail.ts
TRANSLATIONS += ../../../i18n/pl/libmail.ts
TRANSLATIONS += ../../../i18n/pt/libmail.ts
TRANSLATIONS += ../../../i18n/pt_BR/libmail.ts
TRANSLATIONS += ../../../i18n/sl/libmail.ts
TRANSLATIONS += ../../../i18n/zh_CN/libmail.ts
TRANSLATIONS += ../../../i18n/zh_TW/libmail.ts

