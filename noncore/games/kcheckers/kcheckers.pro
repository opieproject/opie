TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= checkers.h \
		  echeckers.h \
		  field.h \
		  kcheckers.h \
		  rcheckers.h
SOURCES		= checkers.cpp \
		  echeckers.cpp \
		  field.cpp \
		  kcheckers.cpp \
		  main.cpp \
		  rcheckers.cpp
INTERFACES	= 
INCLUDEPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR		= $(OPIEDIR)/bin
TARGET		= kcheckers

TRANSLATIONS    = ../../i18n/pt_BR/kcheckers.ts
TRANSLATIONS   += ../../i18n/de/kcheckers.ts
TRANSLATIONS   += ../../i18n/en/kcheckers.ts
TRANSLATIONS   += ../../i18n/hu/kcheckers.ts
TRANSLATIONS   += ../../i18n/sl/kcheckers.ts
TRANSLATIONS   += ../../i18n/pl/kcheckers.ts
TRANSLATIONS   += ../../i18n/ja/kcheckers.ts
TRANSLATIONS   += ../../i18n/ko/kcheckers.ts
TRANSLATIONS   += ../../i18n/no/kcheckers.ts
TRANSLATIONS   += ../../i18n/zh_CN/kcheckers.ts
TRANSLATIONS   += ../../i18n/zh_TW/kcheckers.ts
TRANSLATIONS   += ../../i18n/fr/kcheckers.ts
