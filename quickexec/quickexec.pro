TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= 
SOURCES		= quickexec.cpp
LIBS += -ldl -lqpe
TARGET		= quickexec

TRANSLATIONS = ../i18n/de/quickexec.ts \
	 ../i18n/en/quickexec.ts \
	 ../i18n/es/quickexec.ts \
	 ../i18n/fr/quickexec.ts \
	 ../i18n/hu/quickexec.ts \
	 ../i18n/ja/quickexec.ts \
	 ../i18n/ko/quickexec.ts \
	 ../i18n/no/quickexec.ts \
	 ../i18n/pl/quickexec.ts \
	 ../i18n/pt/quickexec.ts \
	 ../i18n/pt_BR/quickexec.ts \
	 ../i18n/sl/quickexec.ts \
	 ../i18n/zh_CN/quickexec.ts \
	 ../i18n/zh_TW/quickexec.ts
