TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= usermanager.h userdialog.h groupdialog.h passwd.h
SOURCES		= usermanager.cpp userdialog.cpp groupdialog.cpp passwd.h main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie -lcrypt
TARGET		= usermanager
DESTDIR         = $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/usermanager.ts \
	../../../i18n/xx/usermanager.ts \
	../../../i18n/en/usermanager.ts \
	../../../i18n/es/usermanager.ts \
	../../../i18n/fr/usermanager.ts \
	../../../i18n/hu/usermanager.ts \
	../../../i18n/ja/usermanager.ts \
	../../../i18n/ko/usermanager.ts \
	../../../i18n/no/usermanager.ts \
	../../../i18n/pl/usermanager.ts \
	../../../i18n/pt/usermanager.ts \
	../../../i18n/pt_BR/usermanager.ts \
	../../../i18n/sl/usermanager.ts \
	../../../i18n/zh_CN/usermanager.ts \
	../../../i18n/zh_TW/usermanager.ts \
	../../../i18n/it/usermanager.ts \
	../../../i18n/da/usermanager.ts

