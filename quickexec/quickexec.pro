TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= 
SOURCES		= quickexec.cpp
LIBS += -ldl -lqpe
TARGET		= quickexec

TRANSLATIONS = ../i18n/de/quickexec.pro \
	 ../i18n/en/quickexec.pro \
	 ../i18n/es/quickexec.pro \
	 ../i18n/fr/quickexec.pro \
	 ../i18n/hu/quickexec.pro \
	 ../i18n/ja/quickexec.pro \
	 ../i18n/ko/quickexec.pro \
	 ../i18n/no/quickexec.pro \
	 ../i18n/pl/quickexec.pro \
	 ../i18n/pt/quickexec.pro \
	 ../i18n/pt_BR/quickexec.pro \
	 ../i18n/sl/quickexec.pro \
	 ../i18n/zh_CN/quickexec.pro \
	 ../i18n/zh_TW/quickexec.pro
