TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= ../bin
HEADERS		= 
SOURCES		= quickexec.cpp
LIBS += -ldl -lqpe

TRANSLATIONS = ../i18n/de/quickexec.pro.in.ts \
	 ../i18n/en/quickexec.pro.in.ts \
	 ../i18n/es/quickexec.pro.in.ts \
	 ../i18n/fr/quickexec.pro.in.ts \
	 ../i18n/hu/quickexec.pro.in.ts \
	 ../i18n/ja/quickexec.pro.in.ts \
	 ../i18n/ko/quickexec.pro.in.ts \
	 ../i18n/no/quickexec.pro.in.ts \
	 ../i18n/pl/quickexec.pro.in.ts \
	 ../i18n/pt/quickexec.pro.in.ts \
	 ../i18n/pt_BR/quickexec.pro.in.ts \
	 ../i18n/sl/quickexec.pro.in.ts \
	 ../i18n/zh_CN/quickexec.pro.in.ts \
	 ../i18n/zh_TW/quickexec.pro.in.ts
